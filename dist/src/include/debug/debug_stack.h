extern int _stackIdx;
extern int DEBUG_STACK_COLOR;

typedef struct debug_stack {
    char *funcName;
    char *fname;
    void *ref;
    word typeOf;
    int line;
    int pos;
} StackEntry;

status DebugStack_Init(MemCtx *m);
void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos);
#define DebugStack_Push(ref, typeOf) _DebugStack_Push(__func__,__FILE__,((Abstract *)(ref)),((ref) != NULL ? typeOf : 0),__LINE__,0)
void DebugStack_Pop();
int DebugStack_Print();
void DebugStack_SetRef(void *v, word typeOf);
