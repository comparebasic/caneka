extern i32 _stackIdx;
extern i32 DEBUG_STACK_COLOR;

#ifdef DEBUGSTACK
    #define Debug_Push(m, a) DebugStack_Push((m), (a), __func__, __FILE__, __LINE__)
    #define Debug_SetRef(m, a) DebugStack_Push((m), (a), __func__, __FILE__, __LINE__)
    #define Return(m, x) DebugStack_Pop((m)); return (x)
#else
    #define Debug_Push(m, a)  
    #define Debug_SetRef(m, a)
    #define Return(m, x) return (x)
#endif

typedef struct debug_stack {
    Type type;
    word typeOf;
    word _;
    const char *funcName;
    const char *fname;
    void *ref;
    i32 line;
    i32 pos;
} StackEntry;

void DebugStack_SetRef(MemCh *m, void *a, const char *funcName, const char *fname, i32 lineno);
void DebugStack_Push(MemCh *m, void *a, const char *funcName, const char *fname, i32 lineno);
void DebugStack_Pop(MemCh *m);
void DebugStack_Print(MemCh *m, struct buff *bf, word flags);
