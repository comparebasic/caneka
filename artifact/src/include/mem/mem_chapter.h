#define CHAPTER_SIZE 16mb
#define PAGE_SIZE 4096

typedef struct mem_chapter {
    Type type;
    int size;
    void *start;
    MemCtx *m;
    Span *pages;
    Iter it;
};
void *MemChapter_GetBytes(MemChapter *cp, MemCtx *m, i16 level);
status MemChapter_FreeSlab(MemChapter *cp, MemCtx *m, MemSlab *sl);
status MemChapter_Make(MemChapter **cp);
