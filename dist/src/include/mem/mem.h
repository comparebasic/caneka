/*
MemCtx
Memory context

This is the manually-run garbage collector.

related: core/mem.c
*/
#define MEM_SLAB_SIZE 4096

typedef struct mem_slab {
    byte bytes[MEM_SLAB_SIZE];
    word idx;
    i16 level;
    void *addr;
    struct mem_slab *next;
} MemSlab;

typedef struct mem_ctx {
    Type type;
    MemSlab *start_sl;
    int count;
    struct span *index;
    struct {
        void *ptr;
        size_t sz;
        int slabIdx;
        int offset;
    } latest;
} MemCtx;

size_t MemCount();
void *MemCtx_Alloc(MemCtx *m, size_t s);
void *MemCtx_AllocTemp(MemCtx *m, size_t s, i16 level);
void *MemCtx_Realloc(MemCtx *m, size_t s, void *orig, size_t origsize);
MemCtx *MemCtx_Make();
status MemCtx_Free(MemCtx *m);
status MemCtx_FreeTemp(MemCtx *m, i16 level);
i64 MemCtx_Used(MemCtx *m);
void *MemCtx_GetSlab(MemCtx *m, void *addr);
MemSlab *MemSlab_Attach(MemCtx *m, MemSlab *sl);
void *MemSlab_GetStart(MemSlab *sl);
MemSlab *MemSlab_Make(MemCtx *m, i16 level);
