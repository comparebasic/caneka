typedef i64 (*ToStreamFunc)(Stream *sm, Abstract *a, cls type, word flags);
extern Lookup *StreamPrintChain = NULL;
status StreamTo_Init(MemCh *m);
void indent_Print(int indent);
i64 Bits_Print(Stream *sm, byte *bt, size_t length, boolean extended);
i64 FlagStr(word flag, char *dest, char *map);
i64 ToStream(Stream *sm, void *t, cls type, boolean extended);
