extern struct lookup *ToStreamLookup;
extern struct lookup *ToSFlagLookup;
extern struct str **stateLabels;

typedef status (*ToSFunc)(struct buff *bf, void *a, cls type, word flags);

status StreamTo_Init(struct mem_ctx *m);
void indent_Print(int indent);
status ToStream_NotImpl(struct buff *bf, void *a, cls type, word flags);
status _ToStream_NotImpl(char *func, char *file, i32 line, struct buff *bf, void *a, cls type, word flags);
status Bits_PrintArray(struct buff *bf, void *arr, i64 sz, i32 count);
status Bits_Print(struct buff *bf, byte *bt, size_t length, word flags);
status Bits_PrintNum(struct buff *bf, byte *bt, size_t length, word flags);
status Slots_Print(struct buff *bf, util *ut, i32 slots, word flags);
status ToS(struct buff *bf, void *t, cls type, word flags);
status Str_AddFlags(struct str *s, word flags, char *map);
i64 FlagStr(word flag, char *dest, char *map);
