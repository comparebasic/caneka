typedef i64 (*DebugPrintFunc)(Stream *sm, Abstract *a, cls type, boolean extended);

extern struct lookup *DebugPrintChain;
extern char *TypeStrings[];
extern boolean SHOW_SERVE_TESTS;
extern MemCh *_debugM;
extern Stream *DebugOut;

status Debug_Init(MemCh *m);
void indent_Print(int indent);
void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended);
i64 Str_Debug(Stream *sm, void *t, cls type, boolean extended);
