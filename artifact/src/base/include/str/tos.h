extern struct lookup *ToStreamLookup;
extern struct lookup *ToSFlagLookup;
extern Str **stateLabels;

typedef status (*ToSFunc)(struct buff *bf, Abstract *a, cls type, word flags);

status StreamTo_Init(MemCh *m);
void indent_Print(int indent);
i64 ToStream_NotImpl(struct buff *bf, Abstract *a, cls type, word flags);
i64 _ToStream_NotImpl(char *func, char *file, i32 line, struct buff *bf, Abstract *a, cls type, word flags);
i64 Bits_PrintArray(struct buff *bf, void *arr, i64 sz, i32 count);
i64 Bits_Print(struct buff *bf, byte *bt, size_t length, word flags);
i64 Bits_PrintNum(struct buff *bf, byte *bt, size_t length, word flags);
i64 Slots_Print(struct buff *bf, util *ut, i32 slots, word flags);
i64 ToS(struct buff *bf, Abstract *t, cls type, word flags);
i64 Str_AddFlags(Str *s, word flags, char *map);
i64 FlagStr(word flag, char *dest, char *map);
