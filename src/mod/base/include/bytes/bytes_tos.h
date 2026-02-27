status Str_ToSInit(struct mem_ctx *m, struct lookup *lk);
boolean StrVec_Empty(void *_a);
boolean Str_Empty(void *_a);
status Str_InitLabels(struct mem_ctx *m, struct lookup *lk);
status Bytes_Debug(struct buff *bf, byte *start, byte *end);
status Str_Print(struct buff *bf, void *a, cls type, word flags);
status StrVec_Print(struct buff *bf, void *a, cls type, word flags);
