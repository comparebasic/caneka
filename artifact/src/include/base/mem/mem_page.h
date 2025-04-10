typedef struct mem_page {
    Type type;
    i16 level;
    i16 remaining;
} MemPage;

#define MEM_SLAB_SIZE (PAGE_SIZE-sizeof(MemPage))
#define MemPage_Available(sl) (sl)->remaining
#define MemPage_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)

void *MemPage_Alloc(MemPage *pg, word sz);
MemPage *MemPage_Attach(MemCh *m, i16 level);
MemPage *MemPage_Make(MemCh *m, i16 level);
