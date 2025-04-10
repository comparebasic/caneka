typedef struct mem_book {
    Type type;
    i32 size;
    void *start;
    struct iter it;
    struct mem_ctx m;
} MemBook;

MemBook *MemBook_Get(void *addr);
i64 MemCount(i16 level);
i64 MemChapterCount();
i64 MemAvailableChapterCount();
status MemBook_FreePage(MemCh *m, MemPage *pg);
void *MemBook_GetPage(void *addr);
MemBook *MemBook_Make(MemBook *prev);
