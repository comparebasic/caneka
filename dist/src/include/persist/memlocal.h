typedef struct localptr {
    int slabIdx;
    int offset;
} LocalPtr;

typedef struct memlocal_item {
    Type type;
    word typeOf;
    i64 idx;
    Abstract *a;
}MemLocalItem;

#define INDEX_PATHNAME "index"

extern Chain *MemLocalToChain;
extern Chain *MemLocalFromChain;

Span *MemLocal_Make();
status MemLocal_Init(MemCtx *m);

status MemLocal_Persist(MemCtx *m, Span *tbl, String *path, IoCtx *ctx);
Span *MemLocal_Load(MemCtx *m, String *path, struct ioctx *ctx);
status MemLocal_Destroy(MemCtx *m, String *path, IoCtx *ctx);
status MemLocal_GetLocal(MemCtx *m, void *addr, LocalPtr *lptr);
Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr);
