typedef struct mem_book {
    Type type;
    i32 size;
    i32 idx;
    void *start;
    struct iter recycled;
    struct mem_ctx m;
} MemBook;

MemBook *MemBook_Get(void *addr);
MemBook *MemBook_Check(void *addr);
i32 MemBook_GetBookIdx(void *addr);
i32 MemBook_GetPageIdx(void *addr);
i64 MemCount(i16 level);
i64 MemChapterCount();
i64 MemAvailableChapterCount();
status MemBook_FreePage(MemCh *m, MemPage *pg);
void *MemBook_GetPage(void *addr);
MemBook *MemBook_Make(MemBook *prev);
