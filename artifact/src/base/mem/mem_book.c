#include <external.h>
#include <caneka.h>

static int bookIdx = -1;
static MemRange _books[16] = {
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}
};

static MemBook *MemBook_get(void *addr){
    int idx = bookIdx;
    if(addr == NULL){
        return _books[idx].book;
    }
    while(idx >= 0){
        MemRange *mrange = _books+idx;
        if(addr >= mrange->start && addr <= mrange->end){
            return mrange->book;
        }
        idx--;
    }
    Fatal("MemBook not found", TYPE_BOOK);
    return NULL;
}

#ifdef INSECURE
MemBook *MemBook_Get(void *addr){
    return MemBook_get(addr);
}
#endif

i64 MemCount(i16 level){
    i64 total = 0;
    Iter it;
    for(int i = 0; i <= bookIdx; i++){
        MemBook *cp = _books[i].book;
        Iter_Init(&it, &cp->pages);
        while((Iter_Next(&it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&it);
            if(sl != NULL && sl->level >= level){
                total += MemSlab_Taken(sl);
            }
        }
    }
    return total;
}

i64 MemChapterCount(){
    i64 total = 0;
    Iter it;
    for(int i = 0; i <= bookIdx; i++){
        MemBook *cp = _books[i].book;
        total += (i64)cp->pages.nvalues;
    }
    return total;
}

status MemBook_FreeSlab(MemCtx *m, MemSlab *sl){
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    MemBook *cp = MemBook_get(m);
    int idx = (((void *)sl) - cp->start)/PAGE_SIZE;
    return Span_Remove(&cp->pages, idx);
}

void *MemBook_GetBytes(){
    MemBook *cp = MemBook_get(NULL);
    int idx = -1;
    if(cp->pages.metrics.available != -1){
        idx = cp->pages.metrics.available;
        cp->pages.metrics.available = -1;
    }else{
        while((Iter_Next(&cp->it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&cp->it);
            if(sl == NULL){
                idx = cp->it.idx;
                cp->pages.metrics.selected = idx;
                break;
            }
        }
    }
    /*
    printf("it->idx:%d, idx:%d max_idx:%d nvalues:%d\n", cp->it.idx, idx, cp->pages.max_idx, cp->pages.nvalues);
    */
    if(idx == -1 && cp->pages.max_idx+1 < PAGE_COUNT){
        idx = cp->pages.max_idx+1;
    }
    if(idx != -1){
        cp->pages.metrics.selected = idx;
        void *page = cp->start+(idx*PAGE_SIZE);
        memset(page, 0, PAGE_SIZE);
        return page;
    }
    /* make new chapter here as all chapters are full */
    Fatal("Next chapter not implemented", TYPE_BOOK);

    if(MemBook_Make(cp) != NULL){
        return MemBook_GetBytes();
    }
    return NULL;
}

status MemBook_Claim(MemSlab *sl){
    MemBook *cp = MemBook_get(sl);
    if(cp->pages.metrics.selected != -1){
        Span_Set(&cp->pages, cp->pages.metrics.selected, (Abstract *)sl);
        cp->pages.metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}

MemBook *MemBook_Make(MemBook *prev){
    bookIdx++;
    if(bookIdx >= CHAPTER_MAX){
        Fatal("Book already taken", TYPE_BOOK);
        return NULL;
    }
    MemRange *mrange = _books+bookIdx;
    if(mrange->book != NULL){
        Fatal("Book already taken", TYPE_BOOK);
        return NULL;
    }

    void *start = mmap(prev, 
        CHAPTER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    mrange->start = start;
    mrange->end = start+CHAPTER_SIZE-sizeof(void *);

    if(start == MAP_FAILED){
        printf("err:%s\n", strerror(errno));
        Fatal("Unable to map memory", TYPE_BOOK);
        return NULL;
    }

    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = start,
    };

    MemBook *cp = MemSlab_Alloc(&sl, sizeof(MemBook));
    cp->type.of = TYPE_BOOK;
    MemCtx_Setup(&cp->m, &sl);

    cp->start = start;
    mrange->book = cp;

    Span *p = &cp->pages;
    Span_Setup(p);
    p->m = &cp->m;
    p->root = MemSlab_Alloc(&cp->m.first, sizeof(slab));
    Iter_Init(&cp->it, &cp->pages);

    cp->pages.metrics.selected = 0;
    MemBook_Claim(&cp->m.first);
    return cp;
}
