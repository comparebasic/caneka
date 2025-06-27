extern i32 _stackIdx;
extern i32 DEBUG_STACK_COLOR;

typedef struct debug_stack {
    char *funcName;
    char *fname;
    void *ref;
    word typeOf;
    i32 line;
    i32 pos;
} StackEntry;

#define DebugStack_Push(ref, typeOf) _DebugStack_Push((char *)__func__,__FILE__,((Abstract *)(ref)),((ref) != NULL ? typeOf : 0),__LINE__,0)

status DebugStack_Init(MemCh *m);
void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, i32 line, i32 pos);
void DebugStack_Pop();
void DebugStack_SetRef(void *v, word typeOf);
i32 DebugStack_Print(Stream *sm, word flags);
StackEntry *DebugStack_Get();
status DebugStack_Show(Str *style, Str *msg, word flags);
