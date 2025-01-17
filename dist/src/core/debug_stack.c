#include <external.h>
#include <caneka.h>

static Span *stack = NULL;
static void sigH(int sig, siginfo_t *info, void *ptr){
    Fatal("Sig", 0);
    exit(1);
}

static setSigs(){
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

void DebugStack_Push(char *cstr){
    Span_Set(stack, ++_stackIdx, (Abstract*)cstr);
}

void DebugStack_Pop(){
    _stackIdx--;
    Span_Cull(stack, 1);
    MemCtx_TempLevel = _stackIdx;
}

int DebugStack_Print(){
    Iter it;
    Iter_Init(&it, stack);
    while((Iter_Next(&it) & END) == 0){
        void *cstr = (void *)Iter_Get(&it);
        int color = DEBUG_STACK_COLOR;
        if((it.type.state & FLAG_ITER_LAST) != 0){
            color = COLOR_RED;
        }
        printf("    %s\n", cstr);
    }
    return 0;
}
