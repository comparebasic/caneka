#include <external.h>
#include <caneka.h>

static Span *stack = NULL;
static boolean _fuse = TRUE;
static void sigH(i32 sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Sig Seg Fault", NULL);
    }else{
        write(0, "Double SigH\n", strlen("Double SigH\n"));
    }
    exit(1);
}

static void sigI(i32 sig, siginfo_t *info, void *ptr){
    char *cstr = "SigI - exiting\n";
    write(0, cstr, strlen(cstr));
    exit(1);
}

static void setSigs(){
    struct sigaction a;
    struct sigaction b;
    memset(&a, 0, sizeof(struct sigaction));
    a.sa_flags = SA_NODEFER;
    a.sa_sigaction = sigH;
    sigaction(SIGSEGV, &a, NULL);

    memset(&b, 0, sizeof(struct sigaction));
    b.sa_flags = SA_NODEFER;
    b.sa_sigaction = sigI;
    sigaction(SIGINT, &b, NULL);
}

i32 DEBUG_STACK_COLOR = COLOR_GREEN;
i32 _stackIdx = 0;

status DebugStack_Init(MemCh *m){
    _stackIdx = 0;
    stack = Span_Make(m);
    setSigs();

    i32 idx;
    MemPage *pg = MemCh_GetPage(m, stack, &idx);
    return SUCCESS;
}

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, i32 line, i32 pos){
    stack->m->type.range--;

    StackEntry *entry = MemCh_Alloc(stack->m, sizeof(StackEntry));
    entry->funcName = cstr;
    entry->fname = fname;
    entry->ref = ref;
    entry->typeOf = typeOf;
    entry->line = line;
    entry->pos = pos;

    Span_Set(stack, _stackIdx++, (Abstract*)entry);

    stack->m->type.range++;
}

void DebugStack_Pop(){
    _stackIdx--;
    stack->max_idx = _stackIdx;
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Span_Get(stack, _stackIdx-1);
    if(entry == NULL){
        return;
    }
    entry->ref = v;
    entry->typeOf = typeOf;
}

i32 DebugStack_Print(Stream *sm, word flags){
    Iter it;
    Iter_Init(&it, stack);
    boolean first = TRUE;
    while((Iter_Prev(&it) & END) == 0){
        StackEntry *entry = (StackEntry*)it.value;
        if(entry != NULL){
            Stream_Bytes(sm, (byte *)"    \x1b[1;33m", 12);
            Stream_Bytes(sm, (byte *)entry->funcName, strlen(entry->funcName));
            Stream_Bytes(sm, (byte *)"\x1b[22m:", 6);
            Stream_Bytes(sm, (byte *)entry->fname, strlen(entry->fname));
            if(flags & MORE && entry->ref != NULL){
                Stream_Bytes(sm, (byte *)" - \x1b[1m", 8);
                if(entry->typeOf == TYPE_CSTR){
                    Str *s = Str_CstrRef(sm->m, (char *)entry->ref);
                    ToS(sm, (Abstract *)s, s->type.of, MORE);
                }else if(entry->typeOf != 0){
                    ToS(sm, entry->ref, entry->typeOf, MORE|DEBUG);
                }
            }
            Stream_Bytes(sm, (byte *)"\x1b[0m\n", 5);
        }
    }
    return 0;
}
