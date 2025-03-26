typedef struct mem_chapter {
    Type type;
    int size;
    void *start;
    struct iter it;
    struct mem_ctx m;
} MemBook;
void *MemBook_GetBytes();
MemBook *MemBook_Make(MemBook *cp);
status MemBook_FreeSlab(struct mem_ctx *m, struct mem_slab *sl);
status MemBook_Claim(struct mem_slab *sl);
i64 MemCount(i16 level);
#ifdef INSECURE
MemBook *MemBook_Get(void *addr);
#endif
