#include <external.h>
#include <caneka.h>

static int bookIdx = -1;
static i32 pageIdx = 0;
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
        for(i32 j = 0; j < pageIdx && j < PAGE_MAX; j++){
            void *page = book->start+(j*PAGE_SIZE);
            MemPage *sl = (MemPage *)page;
            if(sl->type.of == TYPE_MEMSLAB){
                total += MemPage_Taken(sl); 
            }
        }
    }
    return total;
}

i64 MemChapterCount(){
    return pageIdx;
}

status MemBook_FreePage(MemCh *m, MemPage *pg){
    memset(pg, 0, PAGE_SIZE);

    MemBook *book = MemBook_get(m);
    Iter_Setup(&book->it, book->it.span, SPAN_OP_ADD, book->it.idx);
    book->it.value = pg;
    status r = Iter_Query(&book->it);

    book->it.type.state &= ~SPAN_OP_ADD;
    book->it.type.state |= SPAN_OP_GET;
    return r;
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    i32 idx = -1;
    Iter_Reset(&book->it);
    if(book->it.span->nvalues > 0){
        idx = book->it.span->max_idx;
        Span_Remove(book->it.span, book->it.span->max_idx);
        return book->start+(idx*PAGE_SIZE);
    }else{
        for(i32 i = pageIdx; i < PAGE_MAX; i++){
            void *page = book->start+(i*PAGE_SIZE);
            MemPage *sl = (MemPage *)page;
            if(sl->type.of == 0){
                if(i >= pageIdx){
                    pageIdx = i+1;
                }
                return page;
            }
        }
    }

    /* make new chapter here as all chapters are full */
    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Next MemBook not implemented");

    if((book = MemBook_Make(book)) != NULL){
        return MemBook_GetPage(book);
    }

    Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error making another MemBook");
    return NULL;
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

    if(start == MAP_FAILED){
        printf("err:%s\n", strerror(errno));
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to map memory");
        return NULL;
    }


    MemPage *sl = (MemPage *)start;
    sl->type.of = TYPE_MEMSLAB;
    sl->remaining = MEM_SLAB_SIZE;

    MemBook *book = MemPage_Alloc(sl, sizeof(MemBook));
    book->type.of = TYPE_BOOK;
    
    MemCh_Setup(&book->m, sl);
    MemCh_ReserveSpanExpand(&book->m, sl, 0);

    book->start = start;
    mrange->book = book;

    Span *p = MemPage_Alloc(sl, sizeof(Span));
    Span_Setup(p);
    p->m = &book->m;
    p->root = MemPage_Alloc(sl, sizeof(slab));

    Iter_Setup(&book->it, p, SPAN_OP_GET, 0);
    mrange->book = book;

    return book;
}
