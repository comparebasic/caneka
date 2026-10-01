#include <external.h>
#include "base_module.h"

void MemCh_CountBytes(MemCh *m, i64 *_count){
    i64 count = *_count + (PAGE_SIZE - m->page->remaining);
    Iter_Reset(&m->backlog);
    while((Iter_Next(&m->backlog) & END) == 0){
        MemPage *sl = (MemPage *)m->backlog.value; 
        count += (PAGE_SIZE - sl->remaining);
    }

    *_count = count;

    if(m->nested != NULL){
        Iter_Reset(m->nested);
        while((Iter_Next(m->nested) & END) == 0){
            MemCh_CountBytes(m->nested->value, _count);
        }
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

MemCh *MemCh_Nest(MemCh *m){
    MemCh *nest = MemCh_Make();
    Iter_Add(m->nested, nest); 
    m->level = (i32)m->nested->p->maxIdx;
    return nest;
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
    if(m->nested != NULL){
        while((Iter_Prev(m->nested) & END) == 0 && m->nested->idx >= m->level){
            MemCh_Free(m->nested->value);
            Iter_Remove(m->nested, m->nested->idx);
        }
    }

    MemBook_RecycleAll();
}

void MemCh_Free(MemCh *m){
    Iter_Reset(&m->backlog);
    while((Iter_Next(&m->backlog) & END) == 0){
        MemPage *pg = (MemPage *)m->backlog.value;
        MemBook_FreePage(pg);
    }
    MemBook_FreePage(m->page);
    MemBook_RecycleAll();
}

void MemCh_Init(MemCh *m, MemPage *pg){
    m->type.of = TYPE_MEMCTX;
    m->page = pg;
    Iter_Init(&m->backlog, Span_Make(m, ZERO));
#ifdef DEBUGSTACK
    Iter_Init(&m->debugIt, Span_Make(m, ZERO));
#endif
}

MemCh *MemCh_Make(){
    MemPage *pg = MemPage_Make(NULL);
    MemCh *m = (MemCh *)MemPage_Alloc(pg, sizeof(MemCh));
    MemCh_Init(m, pg);
    return m;
}
