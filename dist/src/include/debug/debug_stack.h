extern int _stackIdx;
extern int DEBUG_STACK_COLOR;

status DebugStack_Init(MemCtx *m);
void DebugStack_Push(char *cstr);
void DebugStack_Pop();
int DebugStack_Print();
