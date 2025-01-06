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
} Ctx;

static Span *p;
static MemCtx *gm;
static Match mt;
static String *shelf;
static PatCharDef pat[] = {{PAT_MANY|PAT_TERM|PAT_INVERT, 0, 0},{PAT_END, 0, 0}};
static int offset;

static status doStuff(MemCtx *m, String *arg, Abstract *a){
    Match_FeedString(&mt, arg, offset);
    printf("Arg recieved: '%s'\n", String_ToChars(m, arg));
    /*
    r |= SubProcess(m, p, String_Make(m, bytes("loop")));
    if((r & SUCCESS) != 0){
        ;
    }
    return SUCCESS;
    */
    return NOOP;
}

int main(int argc, char **argv){
    status r = READY;
    if(argc == 1){
        printf("stuff | %s ${it} will be an environment variable\n", argv[0]);
        exit(1);
    }

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    gm = m;

    Ctx *ctx = MemCtx_Alloc(m, sizeof(Ctx));
    ctx->type.of = TYPE_LOOP_CTX;

    p = Span_Make(m, TYPE_SPAN);
    for(int i = 0; i < argc; i++){
        Span_Add(p, (Abstract *)String_Make(m, bytes(argv[i])));
    }

    shelf = String_Init(m, STRING_EXTEND);
    Match_SetPattern(&mt, pat);
    offset = 0;

    File file;
    File_Init(&file, NULL, NULL, NULL);
    File_StreamWithOpen(m, stdin, &file, NULL, doStuff, (Abstract *)ctx);

    printf("hi\n");

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
