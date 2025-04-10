#include <external.h>
#include <caneka.h>

static Span *stack = NULL;
static void sigH(int sig, siginfo_t *info, void *ptr){
    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Sig Seg Fault");
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

status DebugStack_Init(MemCh *m){
    _stackIdx = 0;
    stack = Span_Make(m);
    setSigs();

    i32 idx;
    MemPage *pg = MemCh_GetPage(m, stack, &idx);
    return SUCCESS;
}

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos){
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
    Span_Cull(stack, 1);
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Span_Get(stack, _stackIdx-1);
    if(entry == NULL){
        return;
    }
    entry->ref = v;
    entry->typeOf = typeOf;
}

int DebugStack_Print(){
    Iter it;
    Iter_Init(&it, stack);
    boolean first = TRUE;
    while((Iter_Next(&it) & END) == 0){
        StackEntry *entry = (StackEntry*)it.value;
        if(entry != NULL){
            Out("    ^D^y_c - _c:_i4^0 ", 
                entry->funcName, entry->fname, entry->line);
            if(entry->ref != NULL && entry->typeOf != 0){
                Out("^y_d^0", entry->ref);
            }else if(entry->ref != NULL && entry->typeOf == 0){
                Out("^y_c^0", entry->ref);
            }
            Out("\n");
        }
    }
    return 0;
}
