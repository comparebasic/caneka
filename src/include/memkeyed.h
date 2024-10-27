MemCtx *MemKeyed_Make(MemCtx *m, Span *tbl);
status MemKeyed_SetKey(MemCtx *m, Abstract *key);
status MemKeyed_Alloc(MemCtx *m, Abstract *key);
status MemKeyed_Persist(MemCtx *m, IoCtx *ctx);
