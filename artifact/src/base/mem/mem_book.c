#include <external.h>
#include <caneka.h>

static i32 bookIdx = -1;
static MemBook *_books[16] = {
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
};

static MemBook *MemBook_check(void *addr){
    i32 idx = bookIdx;
    if(addr == NULL){
        return _books[idx];
    }
    while(idx >= 0){
        MemBook *mb = _books[idx];
        if(addr >= mb->start && addr <= mb->start+BOOK_SIZE){
            return mb;
        }
        idx--;
    }
    return NULL;
}

static MemBook *MemBook_get(void *addr){
    MemBook *book = MemBook_check(addr);
    if(book != NULL){
        return book;
    }
    Abstract *args[] = {
        (Abstract *)Util_Wrapped(ErrStream->m, (util)addr),
        NULL
    };
    Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook not found for \\@@", args);
    return NULL;
}

#ifdef INSECURE
MemBook *MemBook_Get(void *addr){
    return MemBook_get(addr);
}
MemBook *MemBook_Check(void *addr){
    return MemBook_check(addr);
}
i32 MemBook_GetBookIdx(void *addr){
    return bookIdx;
}
i32 MemBook_GetPageIdx(void *addr){
    void *bptr = ((void *)_books[bookIdx]);
    if(addr > bptr+BOOK_SIZE){
        return -1;
    }

    return (addr - bptr) / PAGE_SIZE;
}
#else
void _insecureMemError(void *addr){
    Abstract *args[] = {
        (Abstract *)Ptr_Wrapped(ErrStream->m, addr, 0),
        NULL
    };
    Fatal(FUNCNAME, FILENAME, LINENUMBER, 
        "MemBook is private unless otherwise specified at compile time, $", args);
}

MemBook *MemBook_Get(void *addr){
    _insecureMemError(addr);
    return NULL;
}

i32 MemBook_GetPageIdx(void *addr){
    _insecureMemError(addr);
    return 0;
}
i32 MemBook_GetBookIdx(void *addr){
    _insecureMemError(addr);
    return 0;
}
#endif

i64 MemCount(i16 level){
    i64 total = 0;
    Iter it;
    for(i32 i = 0; i <= bookIdx; i++){
        MemBook *book = _books[i];
        for(i32 j = 0; j < book->idx && j < PAGE_MAX; j++){
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
    MemBook *book = _books[0];
    return book->idx - book->recycled.p->nvalues;
}

i64 MemChapterTotal(){
    return _books[0]->idx;
}

i64 MemAvailableChapterCount(){
    return _books[0]->recycled.p->nvalues;
}

status MemBook_WipePages(void *addr){
    status r = READY;
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    while((Iter_PrevRemove(&book->retired) & (END|NOOP)) == 0){
        void *page = Iter_Get(&book->retired);
        memset(page, 0, PAGE_SIZE);
        r |= Iter_Add(&book->recycled, page);
        i32 idx = ((void *)page - book->start) / PAGE_SIZE;
        if(book->type.state & DEBUG){
            printf("\x1b[36m%d To Recycled\x1b[0m Retired Nvalues:%d max_idx:%d END%d|NOOP%d %p\n",
                idx,
                book->retired.p->nvalues,
                book->retired.p->max_idx,
                book->retired.type.state & END,
                book->retired.type.state & NOOP,
                page
            );
            fflush(stdout);
        }
    }
    return r;
}

status MemBook_FreePage(MemCh *m, MemPage *pg){
    MemBook *book = MemBook_get(m);
    status r = Iter_Add(&book->retired, pg);
    if(book->type.state & DEBUG){
        i32 idx = ((void *)pg - book->start) / PAGE_SIZE;
        printf("\x1b[33m%d Retired Page level:%d\x1b[0m Retired Nvalues:%d max_idx:%d END%d %p\n",
            idx,
            (i32)m->level,
            book->retired.p->nvalues, book->retired.p->max_idx,
            book->retired.type.state & END, pg);
        fflush(stdout);
    }
    return r;
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    if((Iter_PrevRemove(&book->recycled) & (END|NOOP)) == 0){
        void *page = Iter_Get(&book->recycled);
        i32 idx = ((void *)page - book->start) / PAGE_SIZE;
        if(book->type.state & DEBUG){
            printf("\x1b[32m%d From Recycled\x1b[0m Nvalues:%d max_idx:%d END%d %p\n",
                idx,
                book->recycled.p->nvalues, book->recycled.p->max_idx,
                book->recycled.type.state & END, page);
            fflush(stdout);
        }
        if(page == NULL){
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemPage from recycled is null", NULL);
        }
        return page;
    }else{
        if(++book->idx <= PAGE_MAX){
            void *page = book->start+(book->idx*PAGE_SIZE);
            i32 idx = ((void *)page - book->start) / PAGE_SIZE;
            if(book->type.state & DEBUG){
                printf("\x1b[32m%d From New\x1b[0m %p\n", idx, page);
                fflush(stdout);
            }
            return page;
        }
    }

    /* make new chapter here as all chapters are full */
    Fatal(FUNCNAME, FILENAME, LINENUMBER, "Next MemBook not implemented", NULL);

    if((book = MemBook_Make(book)) != NULL){
        return MemBook_GetPage(book);
    }

    Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error making another MemBook", NULL);
    return NULL;
}

status MemBook_GetStats(void *addr, MemBookStats *st){
    MemBook *book = _books[bookIdx];
    st->type.of = TYPE_BOOK_STATS;
    st->type.state = ZERO;
    st->bookIdx = 0;
    st->pageIdx = book->idx;
    st->recycled = book->recycled.p->nvalues;
    st->total = st->pageIdx - st->recycled;
    st->type.state |= SUCCESS;
    return st->type.state;
}

MemBook *MemBook_Make(MemBook *prev){
    bookIdx++;
    if(bookIdx >= BOOK_MAX){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Book idx greater than book max", NULL);
        return NULL;
    }
    MemBook *mb = _books[bookIdx];
    if(mb != NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Book already taken", NULL);
        return NULL;
    }

    void *start = mmap(prev, 
        BOOK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    if(((util)start) & MEM_STASH_MASK){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Mempersist requires pages aligned by the first 11 bits", NULL);
        return NULL;
    }

    if(start == MAP_FAILED){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unable to map memory", NULL);
        return NULL;
    }

    MemPage *pg = (MemPage *)start;
    pg->type.of = TYPE_MEMSLAB;
    pg->remaining = MEM_SLAB_SIZE;

    MemBook *book = MemPage_Alloc(pg, sizeof(MemBook));
    book->start = start;
    book->type.of = TYPE_BOOK;
    _books[bookIdx] = book;
    book->idx = bookIdx;
    
    MemCh_Setup(&book->m, pg);

    Span *p = MemPage_Alloc(pg, sizeof(Span));
    Span_Setup(p);
    p->m = &book->m;
    p->root = (slab *)Bytes_AllocOnPage(pg, sizeof(slab), TYPE_POINTER_ARRAY);
    Iter_Init(&book->retired, p);

    p = MemPage_Alloc(pg, sizeof(Span));
    Span_Setup(p);
    p->m = &book->m;
    p->root = (slab *)Bytes_AllocOnPage(pg, sizeof(slab), TYPE_POINTER_ARRAY);
    Iter_Init(&book->recycled, p);

    return book;
}
