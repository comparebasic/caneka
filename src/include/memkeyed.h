MemCtx *MemKeyed_Make(MemCtx *m);
status MemKeyed_Set(MemCtx *m, Span *tbl, Abstract *key, Abstract *value);
status MemKeyed_Alloc(MemCtx *m, Abstract *key);
status MemKeyed_Persist(MemCtx *m, IoCtx *ctx);
