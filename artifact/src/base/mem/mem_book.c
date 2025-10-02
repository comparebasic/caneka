#include <external.h>
#include <caneka.h>

static i32 bookIdx = -1;
static i32 pageIdx = 0;
static i32 maxPageIdx = 0;
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
    return bookIdx;
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
    i32 available = _books[0]->recycled.p->nvalues;
    return pageIdx - available;
}

i64 MemChapterTotal(){
    return pageIdx;
}

i64 MemAvailableChapterCount(){
    return _books[0]->recycled.p->nvalues;
}

status MemBook_FreePage(MemCh *m, MemPage *pg){
    memset(pg, 0, PAGE_SIZE);

    MemBook *book = MemBook_get(m);
    return Iter_Add(&book->recycled, pg);
}

void *MemBook_GetPage(void *addr){
    MemBook *book = MemBook_get(addr);
    if(book == NULL){
        book = MemBook_get(NULL);
    }
    i32 idx = -1;
    if(book->recycled.p->nvalues > 0){
        i32 idx = book->recycled.p->max_idx;
        void *page = Iter_GetByIdx(&book->recycled, idx);
        Iter_RemoveByIdx(&book->recycled, idx);
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

    if(((util)start) & MEM_PERSIST_MASK){
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

    Iter_Init(&book->recycled, p);

    return book;
}
