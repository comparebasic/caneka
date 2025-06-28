#include <external.h>
#include <caneka.h>

static Iter _it;
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

status DebugStack_Init(MemCh *m){
    Iter_Init(&_it,Span_Make(m));
    setSigs();
    DebugStack_Push(NULL, ZERO);
    Iter_PrevRemove(&_it);
    return SUCCESS;
}

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, i32 line, i32 pos){
    Span *stack = _it.p;
    stack->m->type.range--;

    StackEntry *entry = MemCh_Alloc(stack->m, sizeof(StackEntry));
    entry->type.of = TYPE_DEBUG_STACK_ENTRY;
    entry->funcName = cstr;
    entry->fname = fname;
    entry->ref = ref;
    entry->typeOf = typeOf;
    entry->line = line;
    entry->pos = pos;

    Iter_Push(&_it, (Abstract*)entry);
    stack->m->type.range++;
}

void DebugStack_Pop(){
    _it.type.state |= DEBUG;
    Iter_PrevRemove(&_it);
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Iter_Current(&_it);
    if(entry == NULL){
        return;
    }
    entry->ref = v;
    entry->typeOf = typeOf;
}

StackEntry *DebugStack_Get(){
    return (StackEntry *)Iter_Current(&_it);
}

status DebugStack_Show(Str *style, Str *msg, word flags){
    if(msg == NULL){
        msg = Str_Make(OutStream->m, 0);
    }
    Abstract *args[] = {
        (Abstract *)style,
        /*
        (Abstract *)msg
        */
        NULL
    };
    if(flags & DEBUG){
        Out("$[STACKNAME]:[STACKFILE]: [STACKREF]^0\n", args);
    }else{
        Out("$[STACKNAME]: [STACKREF]^0\n", args);
    }
    return SUCCESS;
}

i32 DebugStack_Print(Stream *sm, word flags){
    i64 total = 0;
    Iter it;
    Iter_Init(&it, _it.p);
    while((Iter_Prev(&it) & END) == 0){
        total += ToS(sm, it.value, 0, flags);
    }
    return total;
}
