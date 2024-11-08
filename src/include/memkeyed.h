typedef struct mem_keyed {
    Type type;
    MemCtx *m;
    Span *tbl;
    Iter it;
} MemKeyed;

MemKeyed *MemKeyed_Make(MemCtx *m);
status MemKeyed_Set(MemCtx *m, MemKeyed *mk, Abstract *key, Abstract *value);
status MemKeyed_Alloc(MemCtx *m, Abstract *key);
status MemKeyed_Persist(MemCtx *m, MemKeyed *mstore, IoCtx *ioctx);
MemKeyed *MemKeyed_FromIndex(MemCtx *m, String *index, IoCtx *ctx);
