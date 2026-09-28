extern Span *MemBook_books;
extern Iter *MemBook_recycled;

byte 

typedef struct mem_book {
    Type type;
    i32 idx;
    struct mem_ctx *m;
    Memory *memory;
    Iter it;
    Iter recycled;
} MemBook;

typedef struct mem_book_stats {
    Type type;
    struct {
        i64 book;
        i64 total;
        i64 recycled;
    } idx;
} MemBookStats;

status MemBook_GetStats(MemBook *book, MemBookStats *st);
status MemBook_FreePage(MemPage *pg);

void *MemBook_GetPage(void *addr);
void MemBook_Free(MemBook *book);

MemBook *MemBook_Make();
