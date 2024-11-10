typedef struct local_ptr {
    int slabIdx;
    int offset;
} LocalPtr;

void *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr, boolean convert);
LocalPtr *MemLocal_GetLocalPtr(MemCtx *m, void *ptr);

MemCtx *MemLocal_Make();
