#include <external.h>
#include <caneka.h>

enum lp_types {
    _LOOP_TYPE_START = _TYPE_CORE_END,
    TYPE_LOOP_CTX,
};

typedef struct lp_ctx {
    Type type;
    int idx;
    String *arg;
    Span *cmd_p;
} Ctx;

static Match mt;
static PatCharDef pat[] = {{PAT_KO|PAT_KO_TERM, '\n', '\n'},{PAT_MANY|PAT_TERM, 0, 255},{PAT_END, 0, 0}};
static int offset;

static status doStuff(MemCtx *m, String *s, Abstract *a){
    status r = READY;
    Ctx *ctx = asIfc(a, TYPE_LOOP_CTX);
    i64 total = 0;
    while(total < s->length){
        Match_SetPattern(&mt, pat);
        status mr = Match_FeedString(&mt, s, offset);

        if((mr & SUCCESS) != 0){
           String *arg = String_Sub(m, s, offset, mt.count);  
           total += String_Length(arg)+mt.tail;
           offset += String_Length(arg)+mt.tail;

           setenv("it", String_ToChars(m, arg), 1);
           if((SubProcess(m, ctx->cmd_p, arg) & SUCCESS) == 0){
                printf("Error running cmd\n");
                return ERROR;
           }
        }
        r |= mr;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

int main(int argc, char **argv){
    status r = READY;
    if(argc == 1){
        printf("stuff | %s ${it} will be an environment variable\n", argv[0]);
        exit(1);
    }

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);

    Ctx *ctx = MemCtx_Alloc(m, sizeof(Ctx));
    ctx->type.of = TYPE_LOOP_CTX;
    ctx->cmd_p = Span_Make(m, TYPE_SPAN);

    for(int i = 1; i < argc; i++){
        Span_Add(ctx->cmd_p, (Abstract *)String_Make(m, bytes(argv[i])));
    }

    offset = 0;

    File file;
    File_Init(&file, NULL, NULL, NULL);
    File_StreamWithOpen(m, stdin, &file, NULL, doStuff, (Abstract *)ctx);

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
