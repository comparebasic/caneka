/*
This is the semi-manually-run garbage collector.
*/
#define MEM_SLAB_SIZE PAGE_SIZE 
extern int MemSlab_Count;

/*
 * Individual slabs of memory that have tracking information for the next
 * available address range.
 *
 */
typedef struct mem_slab {
    Type type;
    i16 level;
    i16 remaining;
    void *bytes;
} MemSlab;

typedef struct mem_ctx {
    RangeType type;
    i32 _;
    Span p;
    MemSlab first;
    Iter it;
} MemCtx;

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
/* Create a MemCtx on an existing memeory page */
MemCtx *MemCtx_OnPage(void *page);
/* Setup a memctx and copy the MemSlab struct into the first slab */
status MemCtx_Setup(MemCtx *m, MemSlab *sl);
/* 
 * Returns the slab that contains a pointer 
 *
 * .addr is the pointer to use to search from the slab address
 * */
void *MemCtx_GetSlab(MemCtx *m, void *addr);
/* 
 * Add a slab to an existing MemCtx
 * */
MemSlab *MemSlab_Attach(MemCtx *m, i16 level);
/* reserve space in a slabs bytes */
void *MemSlab_Alloc(MemSlab *sl, word sz);
/* Make MemSlab */
MemSlab *MemSlab_Make(MemCtx *m, i16 level);
/* Get available space in a slab */
#define MemSlab_Available(sl) (sl)->remaining
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
#define MemSlab_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)

/* This macro sets the memory range to be at level (0) in a way that can be
 * restored: by setting it to negative, the absolute value is where it was
 * before */
#define MemCtx_SetToBase(m) ((m)->type.range = -((m)->type.range));
#define MemCtx_SetFromBase(m) ((m)->type.range = abs((m)->type.range));

/* Function to tracking raw malloc memory count */
void *TrackMalloc(size_t sz, cls t);
/* Function to tracking raw malloc memory count */
void TrackFree(void *p, size_t s);
