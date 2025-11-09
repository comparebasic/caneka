status MemPage_Print(struct buff *sm, void *a, cls type, word flags);
status MemCh_Print(struct buff *sm, void *a, cls type, word flags);
status MemBook_Print(struct buff *sm, void *a, cls type, word flags);
status Mem_ToSInit(MemCh *m, struct lookup *lk);
status Mem_InitLabels(MemCh *m, struct lookup *lk);
status Addr_ToS(struct buff *sm, void *a, word flags);
