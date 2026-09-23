enum mem_page_flags {
    MEM_PAGE_PERSIST_ARRAY = 1 << 8,
};

typedef struct mem_page {
    Type type;
    i32 remaining;
} MemPage;

#define MEM_SLAB_SIZE (PAGE_SIZE-sizeof(MemPage)-(sizeof(Slate)*DIM_MAX))
#define MemPage_Available(sl) (sl)->remaining
#define MemPage_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)

void *MemPage_Alloc(MemPage *pg, quad sz);
MemPage *MemPage_Attach(struct mem_ctx *m);
MemPage *MemPage_Make(struct mem_ctx *m);
