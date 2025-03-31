typedef struct mem_book {
    Type type;
    i32 size;
    void *start;
    struct iter it;
    struct mem_ctx m;
} MemBook;

void *MemBook_GetPage(void *addr);
MemBook *MemBook_Make(MemBook *cp);
status MemBook_FreePage(struct mem_ctx *m, struct mem_page *sl);
i64 MemCount(i16 level);
#ifdef INSECURE
MemBook *MemBook_Get(void *addr);
#endif
