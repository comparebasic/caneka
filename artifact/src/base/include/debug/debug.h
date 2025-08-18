extern char *TypeStrings[];
extern MemCh *_debugM;
extern Stream *DebugOut;
static const size_t FLAG_DEBUG_MAX = 18;
extern struct lookup *TypeStringRanges;

void indent_Print(int indent);
status Debug_Init(MemCh *m);
