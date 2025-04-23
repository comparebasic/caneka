typedef i64 (*DebugPrintFunc)(Stream *sm, Abstract *a, cls type, boolean extended);

extern struct lookup *DebugPrintChain;
extern char *TypeStrings[];
extern boolean SHOW_SERVE_TESTS;
extern MemCh *_debugM;
extern Stream *DebugOut;
static const size_t FLAG_DEBUG_MAX = 18;

status Debug_Init(MemCh *m);
void indent_Print(int indent);
i64 Bits_Print(Stream *sm, byte *bt, size_t length, boolean extended);
i64 Str_Debug(Stream *sm, void *t, cls type, boolean extended);
i64 FlagStr(word flag, char *dest, char *map);
