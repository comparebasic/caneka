/*
This is the semi-manually-run garbage collector.
*/
#define MEM_SLAB_SIZE 4096

enum memctx_flags {
    MEMCTX_TEMP = 1 << 8,
};

extern i16 MemCtx_TempLevel;

/*
 * Individual slabs of memory that have tracking information for the next
 * available address range.
 *
 * .bytes is the available memory
 * .addr is the next starting address
 * .next is a pointer to the next slab
 */
typedef struct mem_slab {
    byte bytes[MEM_SLAB_SIZE];
    word idx;
    i16 level;
    void *addr;
    struct mem_slab *next;
} MemSlab;

/*
 * Main memory context object
 *
 * .start_sl is the first memory slab in a linked list of memory slabs
 */
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
i64 MemCtx_MemCount(MemCtx *m, i16 level);
i64 MemCtx_Total(MemCtx *m, i16 level);

#define MemSlab_GetStart(sl) ((void *)(sl)->bytes)
#define MemSlab_Taken(sl) ((sl)->addr - MemSlab_GetStart((sl)))

