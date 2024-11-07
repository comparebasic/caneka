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
    void *addr;
    struct mem_slab *next;
} MemSlab;

typedef struct mem_ctx {
    Type type;
    MemSlab *start_sl;
    int count;
    Abstract *instance;
    struct span *index;
} MemCtx;

typedef struct mem_handle  {
    Type type;
    struct mem_ctx *m;
} MemHandle;

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
i64 MemCtx_Used(MemCtx *m);

void *MemCtx_GetSlab(MemCtx *m, void *addr);

/* utils */
MemCtx *MemCtx_FromHandle(MemHandle *a);

/* mem-keyed */
MemCtx *MemKeyed_Make(MemCtx *m);
status MemKeyed_SetKey(MemCtx *m, Abstract *key);
status MemKeyed_Alloc(MemCtx *m, Abstract *key);
