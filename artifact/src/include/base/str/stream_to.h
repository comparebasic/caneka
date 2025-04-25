typedef i64 (*StreamPrintFunc)(Stream *sm, Abstract *a, cls type, boolean extended);
i64 Bits_Print(Stream *sm, byte *bt, size_t length, boolean extended);
extern struct lookup *StreamPrintChain;
i64 ToStream(Stream *sm, void *t, cls type, boolean extended);
i64 FlagStr(word flag, char *dest, char *map);
