extern int _stackIdx;
extern int DEBUG_STACK_COLOR;

typedef struct debug_stack {
    char *frame_cstr;
    char *ref_cstr;
} StackEntry;

status DebugStack_Init(MemCtx *m);
void DebugStack_Push(char *cstr);
void DebugStack_Pop();
int DebugStack_Print();
status DebugStack_SetChars(char *cstr);
