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
    stack = Span_MakeInline(m, TYPE_SPAN, sizeof(StackEntry));
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
            int color = DEBUG_STACK_COLOR;
            if(first){
                color = COLOR_RED;
                first = FALSE;
            }
            printf("\x1b[1;%dm    %s\x1b[0;%dm - %s:%d", color,
                entry->funcName,
                color,
                entry->fname,
                entry->line);

            if(entry->ref != NULL && entry->typeOf != 0){
                Debug_Print((void *)entry->ref, entry->typeOf, " - ", color, FALSE);
            }else if(entry->ref != NULL && entry->typeOf == 0){
                printf("\x1b[%dm - %s", color, (char *)entry->ref);
            }
            printf("\x1b[0m\n");
        }
    }
    return 0;
}
