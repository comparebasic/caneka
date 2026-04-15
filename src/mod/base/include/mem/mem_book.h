typedef struct mem_book {
    Type type;
    i32 size;
    i32 idx;
    i32 wipeIdx;
    void *start;
    struct iter retired;
    struct iter recycled;
    struct mem_ctx m;
} MemBook;

typedef struct mem_book_stats {
    Type type;
    i32 bookIdx;
    i32 pageIdx;
    i32 recycled;
    i32 total;
} MemBookStats;

status MemBook_GetStats(void *addr, MemBookStats *st);
MemBook *MemBook_Get(void *addr);
MemBook *MemBook_Check(void *addr);
i32 MemBook_GetBookIdx(void *addr);
i32 MemBook_GetPageIdx(void *addr);
i64 MemCount(i16 level);
i64 MemChapterCount();
i64 MemAvailableChapterCount();
status MemBook_FreePage(MemCh *m, MemPage *pg);
status MemBook_WipePages(void *addr);
void *MemBook_GetPage(void *addr);
MemBook *MemBook_Make(MemBook *prev);
void MemBook_Free(MemBook *book);
