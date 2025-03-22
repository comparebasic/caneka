#include <external.h>
#include <caneka.h>

static Span *stack = NULL;
static void sigH(int sig, siginfo_t *info, void *ptr){
    Fatal("Sig", 0);
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
    b.sa_sigaction = sigH;
    sigaction(SIGINT, &b, NULL);
}

int DEBUG_STACK_COLOR = COLOR_GREEN;
int _stackIdx = 0;

status DebugStack_Init(MemCtx *m){
    _stackIdx = 0;
    stack = Span_Make(m);
    setSigs();
    return SUCCESS;
}

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos){
    StackEntry entry = {
        cstr,
        fname,
        ref,
        typeOf,
        line,
        pos
    };
    Span_Set(stack, _stackIdx++, (Abstract*)&entry);
}

void DebugStack_Pop(){
    _stackIdx--;
    Span_Cull(stack, 1);
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Span_Get(stack, _stackIdx-1);
    entry->ref = v;
    entry->typeOf = typeOf;
}

int DebugStack_Print(){
    Iter it;
    Iter_InitReverse(&it, stack);
    boolean first = TRUE;
    while((Iter_Next(&it) & END) == 0){
        StackEntry *entry = (StackEntry*)Iter_Get(&it);
        if(entry != NULL){
            char *color = "^y.";
            if(first){
                color = "^r.";
                first = FALSE;
            }
            Out(_debugM, "^D_^_c - _c:_i4^0\n", color, entry->funcName, entry->fname, entry->line);
            if(entry->ref != NULL && entry->typeOf != 0){
                Out(_debugM, "_^_d^0", color, entry->ref);
            }else if(entry->ref != NULL && entry->typeOf == 0){
                Out(_debugM, "_^_c^0", color, entry->ref);
            }
            Out(_debugM, "\n");
        }
    }
    return 0;
}
