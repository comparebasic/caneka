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

#define DebugStack_Push(ref, typeOf) _DebugStack_Push((char *)__func__,__FILE__,((Abstract *)(ref)),((ref) != NULL ? typeOf : 0),__LINE__,0)

status DebugStack_Init(MemCh *m);
void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos);
void DebugStack_Pop();
void DebugStack_SetRef(void *v, word typeOf);
int DebugStack_Print();
