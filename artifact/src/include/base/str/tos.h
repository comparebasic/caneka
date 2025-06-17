extern struct lookup *ToStreamLookup;
extern struct lookup *ToSFlagLookup;
extern Str **stateLabels;

typedef i64 (*ToSFunc)(Stream *sm, Abstract *a, cls type, word flags);

status StreamTo_Init(MemCh *m);
void indent_Print(int indent);
i64 ToStream_NotImpl(Stream *sm, Abstract *a, cls type, word flags);
i64 _ToStream_NotImpl(char *func, char *file, i32 line, Stream *sm, Abstract *a, cls type, word flags);
i64 Bits_Print(Stream *sm, byte *bt, size_t length, word flags);
i64 Slots_Print(Stream *sm, util *ut, i32 slots, word flags);
i64 FlagStr(word flag, char *dest, char *map);
i64 ToS(Stream *sm, Abstract *t, cls type, word flags);
i64 Str_AddFlags(Str *s, word flags, char *map);
i64 ToS_FlagLabels(Stream *sm, Abstract *a);
i64 ToS_LabelsFromFlag(Stream *sm, Abstract *a, word flags, Str **labels);
