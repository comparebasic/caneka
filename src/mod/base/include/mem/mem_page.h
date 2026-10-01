enum mem_page_flags {
    MEM_PAGE_PERSIST_ARRAY = 1 << 8,
};

typedef byte Memory[PAGE_SIZE];

typedef struct mem_page {
    Type type;
    i32 remaining;
} MemPage;

#define MEM_SLAB_SIZE (PAGE_SIZE-sizeof(MemPage)-(sizeof(Slate)*DIM_MAX))
#define MemPage_Available(sl) (sl)->remaining
#define MemPage_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)

void *MemPage_Alloc(MemPage *pg, i32 sz);
void MemPage_Init(MemPage *pg);
