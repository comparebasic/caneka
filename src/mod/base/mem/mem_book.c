#include <external.h>
#include "base_module.h"

Span *MemBook_books = NULL;
Iter *MemBook_recycled = NULL;

static MemBook *MemBook_fromAddr(void *){
    MemBook *book = NULL;
    if(book == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Unable to get MemBook from addr", NULL);
        return ERROR;
    }
    return NULL;
}

status MemBook_FreePage(MemCh *m, MemPage *pg){
    MemBook *book = MemBook_fromAddr(m);
    i64 idx = ((void *)pg - book->memory) / PAGE_SIZE;
    Iter_Remove(&book->it, pg);
    Iter_Add(MemBook_recycled, pg);
    return ZERO;
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_fromAddr(addr);
    i64 idx = Iter_NextIdx(&book->it);
    if(idx < 0){
        book = MemBook_Make();
        idx = Iter_NextIdx(&book->it);
    }

    MemPage *page = book->memory[idx];
    Iter_Add(&book->it, pg);

    return pg;
}

status MemBook_GetStats(void *addr, MemBookStats *st){
    return st->type.state;
}

void MemBook_Free(MemBook *book){
    munmap(book->memory, BOOK_SIZE);
}

MemBook *MemBook_Make(){

    void *start = mmap(NULL, 
        BOOK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    if(((util)start) & MEM_STASH_MASK){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "Mempersist requires pages aligned by the first 11 bits", NULL);
        return NULL;
    }

    if(start == MAP_FAILED){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "Unable to map memory", NULL);
        return NULL;
    }

    MemPage *pg = (MemPage *)start;
    MemPage_Init(pg);

    MemBook *book = MemPage_Alloc(pg, sizeof(MemBook));
    book->memory = start;
    book->type.of = TYPE_BOOK;

    Span *p = NULL;
    Slate *sl = NULL;

    if(MemBook_books == NULL){
        p = MemPage_Alloc(pg, sizeof(Span));
        sl = MemPage_Alloc(pg, sizeof(Slate));
        Slate_Init(sl);
        Span_Init(p, ZERO, sl);
        MemBook_books = p;
    }

    MemCh *m = MemPage_Alloc(pg, sizeof(MemCh));
    m->type.of = TYPE_MEMCTX;
    m->page = (MemPage *)book->memory[1];

    p = MemPage_Alloc(pg, sizeof(Span));
    sl = MemPage_Alloc(pg, sizeof(Slate));
    Slate_Init(sl);
    Span_Init(p, ZERO, sl);
    Iter_Init(&m->backlog, p);

#ifdef DEBUGSTACK
    p = MemPage_Alloc(pg, sizeof(Span));
    sl = MemPage_Alloc(pg, sizeof(Slate));
    Slate_Init(sl);
    Span_Init(p, ZERO, sl);
    Iter_Init(&m->debutIt, p);
#endif

    book->m = m;

    book->idx = Span_Add(MemBook_books, pg);
    Iter_Add(&book->it, pg);
    Iter_Add(&book->it, m->page);

    return book;
}
