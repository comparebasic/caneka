typedef struct mem_chapter {
    Type type;
    int size;
    void *start;
    struct span pages;
    struct iter it;
    struct mem_ctx m;
} MemChapter;
void *MemChapter_GetBytes();
MemChapter *MemChapter_Make(MemChapter *cp);
status MemChapter_FreeSlab(struct mem_ctx *m, struct mem_slab *sl);
status MemChapter_Claim(struct mem_slab *sl);
i64 MemCount(i16 level);
#ifdef INSECURE
MemChapter *MemChapter_Get(void *addr);
#endif
