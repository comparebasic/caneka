typedef struct local_ptr {
    int slabIdx;
    int offset;
} LocalPtr;

extern Chain *MemLocalChain;

MemCtx *MemLocal_Make();
status MemLocal_Init(MemCtx *m);
LocalPtr *MemLocal_GetLocal(MemCtx *m, void *addr);
MemCtx *MemLocal_FromIndex(MemCtx *m, struct string *index, struct ioctx *ctx);
status MemLocal_Persist(MemCtx *m, MemCtx *mstore, struct ioctx *ioctx);
