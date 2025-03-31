typedef struct mem_page {
    Type type;
    i16 level;
    i16 remaining;
    void *bytes;
} MemPage;

MemPage *MemPage_Attach(struct mem_ctx *m, i16 level);
void *MemPage_Alloc(MemPage *sl, word sz);
MemPage *MemPage_Make(struct mem_ctx *m, i16 level);
#define MemPage_Available(sl) (sl)->remaining
#define MemPage_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)
