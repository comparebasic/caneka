extern char *TypeStrings[];
extern MemCh *_debugM;
extern Stream *DebugOut;
static const size_t FLAG_DEBUG_MAX = 18;

void indent_Print(int indent);
status Debug_Init(MemCh *m);
