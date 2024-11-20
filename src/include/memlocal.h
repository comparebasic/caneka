typedef struct mem_local {
    Type type;
    MemCtx *m;
    Span *tbl;
} MemLocal;

typedef struct local_ptr {
    int slabIdx;
    int offset;
} LocalPtr;

MemLocal *MemLocal_Make(MemCtx *m);
MemLocal *MemLocal_MakeBare(MemCtx *m);
status MemLocal_Awake(MemLocal *ml);
Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr);
status MemLocal_GetLocal(MemCtx *m, void *addr, LocalPtr *lptr);
MemLocal *MemLocal_Load(MemCtx *m, struct ioctx *ctx);
status MemLocal_Persist(MemCtx *m, MemLocal *mstore, struct ioctx *ioctx);
Abstract *MemLocal_Alloc(MemLocal *ml, cls inst, size_t sz, String *key);
Abstract *MemLocal_Set(MemLocal *ml, String *key, Abstract *a);
Abstract *MemLocal_Trans(MemCtx *m, Abstract *a);
status MemLocal_Destroy(MemCtx *m, IoCtx *ctx);
