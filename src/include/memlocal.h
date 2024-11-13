typedef struct mem_local {
    Type type;
    MemCtx *m;
    Span *tbl;
} MemLocal;

typedef struct local_ptr {
    int slabIdx;
    int offset;
} LocalPtr;

MemLocal *MemLocal_Make();
Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr);
LocalPtr *MemLocal_GetLocal(MemCtx *m, void *addr);
MemLocal *MemLocal_FromIndex(MemCtx *m, struct string *index, struct ioctx *ctx);
status MemLocal_Persist(MemCtx *m, MemLocal *mstore, struct ioctx *ioctx);
Abstract *MemLocal_Alloc(MemLocal *ml, cls inst, size_t sz, String *key);
Abstract *MemLocal_Set(MemLocal *ml, String *key, Abstract *a);
Abstract *MemLocal_Trans(MemCtx *m, Abstract *a);
