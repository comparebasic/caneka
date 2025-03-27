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
        printf("returning book %d starting at:%ld\n", idx, (util)_books[idx].start);
        return _books[idx].book;
    }
    while(idx >= 0){
        MemRange *mrange = _books+idx;
        if(addr >= mrange->start && addr <= mrange->end){
            return mrange->book;
        }
        idx--;
    }
    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "MemBook not found");
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
        MemBook *book = _books[i].book;
        Iter_Init(&it, book->it.span);
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
        MemBook *book = _books[i].book;
        total += (i64)book->it.span->nvalues;
    }
    return total;
}

status MemBook_FreeSlab(MemCh *m, MemSlab *sl){
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    MemBook *book = MemBook_get(m);
    int idx = (((void *)sl) - book->start)/PAGE_SIZE;
    return Span_Remove(book->it.span, idx);
}

void *MemBook_GetPage(void *addr){
    printf("Getting Page close to %lu\n", (util)addr);
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    i32 idx = -1;
    printf("Getting Page II\n");
    if(book->it.metrics.available != -1){
        idx = book->it.metrics.available;
        book->it.metrics.available = -1;
    }else{
        printf("Getting Page III\n");
        while((Iter_Next(&book->it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&book->it);
            if(sl == NULL){
                idx = book->it.idx;
                book->it.metrics.selected = idx;
                break;
            }
        }
    }
    if(idx == -1 && book->it.span->max_idx+1 < PAGE_COUNT){
        idx = book->it.span->max_idx+1;
    }
    printf("Getting Page %d\n", idx);
    if(idx != -1){
        book->it.metrics.selected = idx;
        void *page = book->start+(idx*PAGE_SIZE);
        memset(page, 0, PAGE_SIZE);
        return page;
    }
    /* make new chapter here as all chapters are full */
    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Next MemBook not implemented");

    if((book = MemBook_Make(book)) != NULL){
        return MemBook_GetPage(book);
    }

    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error making another MemBook");
    return NULL;
}

status MemBook_Claim(MemSlab *sl){
    MemBook *book = MemBook_get(sl);
    if(book->it.metrics.selected != -1){
        Span_Set(book->it.span, book->it.metrics.selected, (Abstract *)sl);
        book->it.metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}

MemBook *MemBook_Make(MemBook *prev){
    bookIdx++;
    if(bookIdx >= CHAPTER_MAX){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Book already taken");
        return NULL;
    }
    MemRange *mrange = _books+bookIdx;
    if(mrange->book != NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Book already taken");
        return NULL;
    }

    void *start = mmap(prev, 
        CHAPTER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    mrange->start = start;
    mrange->end = start+CHAPTER_SIZE-sizeof(void *);
    printf("Setting up bookIdx:%d\n", bookIdx);

    if(start == MAP_FAILED){
        printf("err:%s\n", strerror(errno));
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to map memory");
        return NULL;
    }

    MemSlab _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = start,
    };

    MemSlab *sl = MemSlab_Alloc(&_sl, sizeof(MemSlab));
    memcpy(sl, &_sl, sizeof(MemSlab));

    MemBook *book = MemSlab_Alloc(sl, sizeof(MemBook));
    book->type.of = TYPE_BOOK;
    MemCh_Setup(&book->m, sl);
    MemCh_ReserveSpanExpand(&book->m, sl, 0);

    book->start = start;
    mrange->book = book;

    Span *p = MemSlab_Alloc(sl, sizeof(Span));
    Span_Setup(p);
    p->m = &book->m;
    p->root = MemSlab_Alloc(sl, sizeof(slab));
    Iter_Init(&book->it, p);
    book->it.span = p;

    book->it.metrics.selected = 0;
    MemBook_Claim(sl);
    return book;
}
