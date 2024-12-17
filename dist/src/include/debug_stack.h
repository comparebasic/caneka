typedef struct debug_stack {
    Type type;
    int idx;
    String name;
    String file;
    int line;
    Abstract *a;
} DebugStackEntry;

#define DEBUG_STACK_MAX 51
extern DebugStackEntry _stackDebug[15+1];
extern int _stackIdx;

extern int DEBUG_STACK_COLOR;

status DebugStack_Init();
void _DebugStack_Push(byte *name, Abstract *a, char *file, int line);
#define DebugStack_Push(name, a) _DebugStack_Push((name), (a), __FILE__, __LINE__)
void DebugStack_Pop();
int DebugStack_Print();
void DebugStack_SetRef(Abstract *a);
