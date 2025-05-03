#include <external.h>
#include <caneka.h>

static i32 bookIdx = -1;
static i32 pageIdx = 0;
static MemBook *_books[16] = {
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
};

static MemBook *MemBook_get(void *addr){
    i32 idx = bookIdx;
    if(addr == NULL){
        return _books[idx];
    }
    while(idx >= 0){
        MemBook *mb = _books[idx];
        if(addr >= mb->start && addr <= mb->start+CHAPTER_SIZE){
            return mb;
        }
        idx--;
    }
    Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook not found", NULL);
    return NULL;
}

#ifdef INSECURE
MemBook *MemBook_Get(void *addr){
    return MemBook_get(addr);
}
i32 MemBook_GetBookIdx(void *addr){
    return bookIdx;
}
i32 MemBook_GetPageIdx(void *addr){
    return pageIdx;
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

i64 MemAvailableChapterCount(){
    return _books[0]->recycled.span->nvalues;
}

status MemBook_FreePage(MemCh *m, MemPage *pg){
    memset(pg, 0, PAGE_SIZE);

    MemBook *book = MemBook_get(m);
    Iter_Setup(&book->recycled, book->recycled.span, SPAN_OP_ADD, book->recycled.idx);
    book->recycled.value = pg;
    status r = Iter_Query(&book->recycled);

    book->recycled.type.state &= ~SPAN_OP_ADD;
    book->recycled.type.state |= SPAN_OP_GET;

    return r;
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    i32 idx = -1;
    Iter_Reset(&book->recycled);
    if(book->recycled.span->nvalues > 0){
        idx = book->recycled.span->max_idx;
        void *page = Span_Get(book->recycled.span, idx);
        Span_Remove(book->recycled.span, idx);
        return page;
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
    Fatal(FUNCNAME, FILENAME, LINENUMBER, "Next MemBook not implemented", NULL);

    if((book = MemBook_Make(book)) != NULL){
        return MemBook_GetPage(book);
    }

    Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error making another MemBook", NULL);
    return NULL;
}

MemBook *MemBook_Make(MemBook *prev){
    bookIdx++;
    if(bookIdx >= CHAPTER_MAX){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Book already taken", NULL);
        return NULL;
    }
    MemBook *mb = _books[bookIdx];
    if(mb != NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Book already taken", NULL);
        return NULL;
    }

    void *start = mmap(prev, 
        CHAPTER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

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
    
    MemCh_Setup(&book->m, pg);

    Span *p = MemPage_Alloc(pg, sizeof(Span));
    Span_Setup(p);
    p->m = &book->m;
    p->root = MemPage_Alloc(pg, sizeof(slab));

    Iter_Setup(&book->recycled, p, SPAN_OP_GET, 0);

    return book;
}
