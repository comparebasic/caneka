i64 MemPage_Print(struct stream *sm, Abstract *a, cls type, word flags);
i64 MemCh_Print(struct stream *sm, Abstract *a, cls type, word flags);
i64 MemBook_Print(struct stream *sm, Abstract *a, cls type, word flags);
status Mem_ToSInit(MemCh *m, struct lookup *lk);
status Mem_InitLabels(MemCh *m, struct lookup *lk);
i64 Addr_ToS(struct stream *sm, void *a, word flags);
