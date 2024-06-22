#define MEM_SLAB_SIZE 4096

typedef struct mem_slab {
    byte bytes[MEM_SLAB_SIZE];
    void *addr;
    struct mem_slab *next;
} MemSlab;

typedef struct mem_ctx {
    MemSlab *start_sl;
    void *instance;
} MemCtx;

#define MemCtx_Bind(m, inst) do { inst->m = m; m->instance = (void *)inst;} while(0);

size_t MemCount();
void *MemCtx_Alloc(MemCtx *m, size_t s);
void *MemCtx_Realloc(MemCtx *m, size_t s, void *orig, size_t origsize);
MemCtx *MemCtx_Make();
status MemCtx_Free(MemCtx *m);
void *MemSlab_GetStart(MemSlab *sl);
MemSlab *MemSlab_Make(MemCtx *m);
size_t MemSlab_Available(MemSlab *sl);
void *MemSlab_Alloc(MemSlab *sl, size_t s);
