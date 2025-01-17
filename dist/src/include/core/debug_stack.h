#define DEBUG_STACK_MAX 51
extern void *_stackDebug[DEBUG_STACK_MAX+1];
extern int _stackIdx;
extern int DEBUG_STACK_COLOR;

status DebugStack_Init(MemCtx *m);
void DebugStack_Push(char *cstr);
void DebugStack_Pop();
int DebugStack_Print();
