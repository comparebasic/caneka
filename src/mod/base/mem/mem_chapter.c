#include <external.h>
#include "base_module.h"

static MemPage *MemCh_AddPage(MemCh *m, i16 level){
    MemPage *pg = MemPage_Make(m, level);
    Iter_Add(&m->it, (void *)pg);
    if(m->it.p->count > m->metrics.totalCeiling){
        m->metrics.totalCeiling = m->it.p->count;
    }
    return pg;
}

void MemCh_CountBytes(MemCh *m, i64 *_count){
    Iter it;
    memcpy(&it, &m->it, sizeof(Iter));
    Iter_Reset(&it);
    i64 count = 0;
    while((Iter_Next(&it) & END) == 0){
        MemPage *sl = (MemPage *)it.value; 
        count += (PAGE_SIZE - sl->remaining);
    }
    *_count = count;
}

void *MemCh_Alloc(MemCh *m, word sz){

    if(sz > MEM_SLAB_SIZE || m == NULL || m->type.of != TYPE_MEMCTX){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, 
            "Error with allocation size or MemCh is NULL not of type MemCh", NULL);
        return NULL;
    }

    i16 level = 0;
    if((m->type.state & MEMCH_BASE) == 0){
        level = m->level;
    }

    MemPage *sl = NULL;
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *_sl = (MemPage *)m->it.value;
        if(_sl != NULL && (_sl->level == level) && _sl->remaining >= sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemCh_AddPage(m, level);
    }

    return MemPage_Alloc(sl, sz);
}

void *MemCh_Realloc(MemCh *m, word s, void *orig, word origsize){
    if(s > origsize){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "Asking to copy more than newly allocated", NULL);
        return NULL;
    }
    void *p = MemCh_Alloc(m, (word)s);
    memcpy(p, orig, origsize);
    return p; 
}

status MemCh_FreeTemp(MemCh *m){
    status r = ZERO;
    i16 level = m->level+1;

    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && pg->level >= level){
            r |= MemBook_FreePage(m, pg);
            r |= Iter_Remove(&m->it, m->it.idx);
        }
    }

    MemBook_WipePages(m);
    return r;
}

status MemCh_Free(MemCh *m){
    status r = ZERO;
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL){
            r |= MemBook_FreePage(m, pg);
        }
    }
    MemBook_WipePages(m);
    return r;
}

status MemCh_Setup(MemCh *m, MemPage *pg){
    m->type.of = TYPE_MEMCTX;
    Span *p = MemPage_Alloc(pg, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->m = m;
    p->root = (Slab *)Bytes_AllocOnPage(pg, sizeof(Slab), TYPE_POINTER_ARRAY);

    Iter_Init(&m->it, p);
    Iter_Set(&m->it, 0, (void *)pg);
    m->it.type.state = ((m->it.type.state & NORMAL_FLAGS) | ITER_GET);
    return ZERO;
}

MemCh *MemCh_OnPage(){
    MemPage *sl = (MemPage *)MemPage_Make(NULL, 0);
    MemCh *m = (MemCh *)MemPage_Alloc(sl, sizeof(MemCh));
    MemCh_Setup(m, sl);
    return m;
}

MemCh *MemCh_Make(){
    MemCh *m = MemCh_OnPage();
#ifdef DEBUGSTACK
    Iter_Init(&m->debugIt, Span_Make(m));
#endif
    return m;
}
