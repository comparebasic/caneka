#include <external.h>
#include "base_module.h"

static boolean _init = FALSE;
Iter MemBook_books;
Iter MemBook_recycled;

static MemBook *MemBook_fromAddr(void *t){
    MemBook *book = NULL;
    Iter_Reset(&MemBook_books);
    while((Iter_Prev(&MemBook_books) & END) == 0){
        MemBook *_b = MemBook_books.value;
        void *start = (void *)_b->memory;
        void *end = ((void *)_b->memory) + sizeof(Memory) - 1;
        if(t >= start && t <= end){
            book = _b;
            break;
        }
    }

    if(book == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Unable to get MemBook from addr", NULL);
        return NULL;
    }

    return book;
}

void MemBook_Recycle(MemBook *book){
    while((Iter_Prev(&book->recycled) & END) == 0){
        MemPage *pg = book->recycled.value;
        i64 idx = ((void *)pg - (void *)book->memory) / PAGE_SIZE;
        if(idx < 0 || idx > PAGE_MAX){
            Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
                "Incompatible page idx to wipe", NULL);
        }
        Iter_Remove(&book->recycled, book->recycled.idx);
        Iter_Remove(&book->it, idx);
    }
}

void MemBook_RecycleAll(){
    MemBook *book = NULL;
    Iter_Reset(&MemBook_books);
    while((Iter_Prev(&MemBook_books) & END) == 0){
        MemBook *book = MemBook_books.value;
        if(book->type.state & MORE){
            MemBook_Recycle(book);
        }
    }
}

status MemBook_FreePage(MemPage *pg){
    MemBook *book = MemBook_fromAddr(pg);
    i64 idx = ((void *)pg - (void *)book->memory) / PAGE_SIZE;
    if(idx < 0 || idx > PAGE_MAX){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Incompatible page idx to recycle", NULL);
    }
    Iter_Add(&book->recycled, pg);
    book->recycled.type.state |= MORE;
    return ZERO;
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_fromAddr(addr);
    i64 idx = Iter_NextIdx(&book->it);
    if(idx < 0){
        book = MemBook_Make();
        idx = Iter_NextIdx(&book->it);
    }

    Memory *mem = book->memory+idx;
    MemPage_Init((MemPage *)mem);
    Iter_Add(&book->it, mem);

    return (MemPage *)mem;
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

    if(!_init){
        _init = TRUE;
        p = MemPage_Alloc(pg, sizeof(Span));
        sl = MemPage_Alloc(pg, sizeof(Slate));
        Slate_Init(sl);
        Span_Init(NULL, p, ZERO, sl);
        Iter_Init(&MemBook_books, p);

        p = MemPage_Alloc(pg, sizeof(Span));
        sl = MemPage_Alloc(pg, sizeof(Slate));
        Slate_Init(sl);
        Span_Init(NULL, p, ZERO, sl);
        Iter_Init(&MemBook_recycled, p);
    }

    MemCh *m = MemPage_Alloc(pg, sizeof(MemCh));
    m->type.of = TYPE_MEMCTX;
    m->page = (MemPage *)book->memory[1];
    MemBook_recycled.p->m = m;
    MemBook_books.p->m = m;

    p = MemPage_Alloc(pg, sizeof(Span));
    sl = MemPage_Alloc(pg, sizeof(Slate));
    Slate_Init(sl);
    Span_Init(m, p, ZERO, sl);
    Iter_Init(&m->backlog, p);

#ifdef DEBUGSTACK
    p = MemPage_Alloc(pg, sizeof(Span));
    sl = MemPage_Alloc(pg, sizeof(Slate));
    Slate_Init(sl);
    Span_Init(m, p, ZERO, sl);
    Iter_Init(&m->debugIt, p);
#endif

    book->m = m;

    Iter_Add(&MemBook_books, pg);
    book->idx = MemBook_books.idx;
    Iter_Add(&book->it, pg);
    Iter_Add(&book->it, m->page);

    return book;
}
