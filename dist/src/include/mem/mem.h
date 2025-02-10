/*
This is the semi-manually-run garbage collector.
*/
#define MEM_SLAB_SIZE 4096
extern int MemSlab_Count;

/*
 * Individual slabs of memory that have tracking information for the next
 * available address range.
 *
 * .bytes is the available memory
 * .addr is the next starting address
 * .next is a pointer to the next slab
 */
typedef struct mem_slab {
    Type type;
    word idx;
    i16 level;
    void *addr;
    byte bytes[MEM_SLAB_SIZE];
    struct mem_slab *next;
} MemSlab;

/*
 * Main memory context object
 *
 * .start_sl is the first memory slab in a linked list of memory slabs
 */
typedef struct mem_ctx {
    RangeType type;
    MemSlab *start_sl;
    int count;
    Abstract *owner;
    struct {
        void *ptr;
        size_t sz;
        int slabIdx;
        int offset;
    } latest;
} MemCtx;

size_t MemCount();
/* Main call for allocating a section of memory
 *
 * .s is the size of bytes to allocate 
 * */
void *MemCtx_Alloc(MemCtx *m, size_t s);
/* Constructor with a flag for temporary storage
 *
 * .level is used to ensure memory is stored where it can be wiped when that
 * level is not longer used */
void *MemCtx_AllocTemp(MemCtx *m, size_t s, i16 level);
/* Similar to MemCtx_Alloc but it will copy an amount from a previous memory
 * segment and fill that into the new space 
 *
 * .s size in bytes to reserve
 * .orig position of the original bytes of memory
 * .origsize amount to copy from the original location
 * */
void *MemCtx_Realloc(MemCtx *m, size_t s, void *orig, size_t origsize);
/* Make */
MemCtx *MemCtx_Make();
/* Free wipes the slags that were allocated and then calls free on the MemCtx
 * struct itself */
status MemCtx_Free(MemCtx *m);
/* Free only slabs tagged as over a certain level */
status MemCtx_FreeTemp(MemCtx *m, i16 level);
/* 
 * Returns the amount in bytes used by the parent allocator (usually malloc) */
i64 MemCtx_Used(MemCtx *m);
/* 
 * Returns the slab that contains a pointer 
 *
 * .addr is the pointer to use to search from the slab address
 * */
void *MemCtx_GetSlab(MemCtx *m, void *addr);
/* 
 * Add a slab to an existing MemCtx
 *
 * .m MemCtx to add to
 * .sl slab to add to the MemCtx .m
 * */
MemSlab *MemSlab_Attach(MemCtx *m, MemSlab *sl);
/* Make MemSlab */
MemSlab *MemSlab_Make(MemCtx *m, i16 level);
/* Get available space in a slab */
size_t MemSlab_Available(MemSlab *sl);
/* Returns the amount of bytes used in this level across multiple slabs in one
 * MemCtx
 * 
 * .m MemCtx to search
 * .level Level that slabs are tagged to to include in the count
 * */
i64 MemCtx_MemCount(MemCtx *m, i16 level);
/* Returns the amount of bytes used or unused (the whole sllab) in this level across multiple slabs in one
 * MemCtx
 * 
 * .m MemCtx to search
 * .level Level that slabs are tagged to to include in the count
 * */
i64 MemCtx_Total(MemCtx *m, i16 level);

/* Gets the amount of bytes used in a particular slab */
#define MemSlab_Taken(sl) ((sl)->addr - (void *)(sl)->bytes)

