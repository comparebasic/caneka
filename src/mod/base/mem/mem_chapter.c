#include <external.h>
#include "base_module.h"

void MemCh_CountBytes(MemCh *m, i64 *_count){
    i64 count = *_count + (PAGE_SIZE - m->page->remaining);
    Iter_Reset(&m->backlog);
    while((Iter_Next(&it) & END) == 0){
        MemPage *sl = (MemPage *)it.value; 
        count += (PAGE_SIZE - sl->remaining);
    }

    *_count = count;

    if(m->next != NULL){
        MemCh_CountBytes(m->next, _count);
    }
}

void *MemCh_Realloc(MemCh *m, quad sz, void *orig, quad origsize){
    if(sz < origsize){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Asking to copy more than newly allocated", NULL);
        return NULL;
    }

    void *p = MemCh_Alloc(m, sz);
    memcpy(p, orig, origsize);
    return p; 
}

MemCh *MemCh_Nest(MemCh *m, quad sz){
    MemCh *latest = m->next;
    while(m->next != NULL){
        latest = m->next;
    }

    MemCh *next = MemCh_Make();
    latest->next = next;
    return next;
}

void *MemCh_Alloc(MemCh *m, quad sz){

    return malloc(sz);

    if(sz > MEM_SLAB_SIZE || m == NULL || m->type.of != TYPE_MEMCTX){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Error with allocation size or MemCh is NULL not of type MemCh", NULL);
        return NULL;
    }

    if(m->page->remaining < sz){
        MemPage *page = m->page;
        m->page = MemPage_Make(m);
        Iter_Add(&m->backlog, page); 
    }

    return MemPage_Alloc(m->page, sz);
}

void MemCh_FreeTemp(MemCh *m){
    MemCh *temp = m->next;
    MemCh *next = temp;
    while(next != NULL){
        next = temp->next;
        MemCh_Free(temp);
        temp = next;
    }

    MemBook_WipePages(m);
}

status MemCh_Free(MemCh *m){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        MemBook_FreePage(m, pg);
    }
    MemBook_FreePage(m, m->page);
    MemBook_WipePages(m);
    return r;
}

void MemCh_Init(MemCh *m){
    m->type.of = TYPE_MEMCTX;
    m->page = pg;
    Iter_Init(&m->backlog, Span_Make(m));
#ifdef DEBUGSTACK
    Iter_Init(&m->debugIt, Span_Make(m));
#endif
}

MemCh *MemCh_Make(){
    MemPage *pg = MemPage_Make(NULL);
    MemCh *m = (MemCh *)MemPage_Alloc(pg, sizeof(MemCh), TYPE_MEMCTX);
    MemCh_Init(m);
    return m;
}
