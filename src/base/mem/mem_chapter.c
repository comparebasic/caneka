#include <external.h>
#include "base_module.h"

static MemPage *MemCh_AddPage(MemCh *m, i16 level){
    MemPage *pg = MemPage_Make(m, level);
    Iter_Add(&m->it, (void *)pg);
    if(m->it.p->nvalues > m->metrics.totalCeiling){
        m->metrics.totalCeiling = m->it.p->nvalues;
    }
    return pg;
}

void *MemCh_Alloc(MemCh *m, size_t sz){
    return MemCh_AllocOf(m, sz, 0);
}

void *MemCh_AllocOf(MemCh *m, size_t sz, cls typeOf){
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh is NULL", NULL);
        return NULL;
    }
    if(m->type.of != TYPE_MEMCTX){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh is missing type.of", NULL);
        return NULL;
    }
    if(sz > MEM_SLAB_SIZE){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Trying to allocation too much memory at once", NULL);
    }

    if(m->it.p == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Whaaaaat m->it.p is NULL?", NULL);
    }

    if(!Guard(&m->guard, MEM_GUARD_MAX, "MemCh_Alloc", FILENAME, LINENUMBER)){
        byte _b[128];
        memset(_b, 0, 128);
        Str s;
        s.type.of = TYPE_STR;
        s.bytes = _b;
        s.alloc = 128;
        Str_AddCstr(&s, "Guard Error allocating ");
        Str_AddCstr(&s, Type_ToChars(typeOf));
        Fatal(FUNCNAME, FILENAME, LINENUMBER, (char *)s.bytes, NULL);
        return NULL;
    }

    i16 level = 0;
    if((m->type.state & MEMCH_BASE) == 0){
        level = m->level;
    }

    word _sz = (word)sz;

    MemPage *sl = NULL;
    word prev = m->type.state;
    m->type.state |= MEMCH_BASE;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *_sl = (MemPage *)m->it.value;
        if(_sl != NULL && (level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemCh_AddPage(m, level);
    }

    if(sl->level != level){
        sl = MemCh_AddPage(m, level);
    }

    m->it.type.state = (m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    Iter_Reset(&m->it);

    Guard_Reset(&m->guard);
    m->type.state = prev;
    return MemPage_Alloc(sl, _sz);
}

void *MemCh_Realloc(MemCh *m, size_t s, void *orig, size_t origsize){
    if(s > origsize){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Asking to copy more than newly allocated", NULL);
        return NULL;
    }
    void *p = MemCh_Alloc(m, s);
    memcpy(p, orig, origsize);
    return p; 
}

status MemCh_FreeTemp(MemCh *m){
    status r = READY;
    i16 level = m->level+1;

    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && pg->level >= level){
            r |= MemBook_FreePage(m, pg);
            r |= Iter_Remove(&m->it);
        }
    }

    MemBook_WipePages(m);
    return r;
}

status MemCh_Free(MemCh *m){
    status r = READY;
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
    Span_Setup(p);
    p->m = m;
    p->max_idx = -1;
    p->root = (slab *)Bytes_AllocOnPage(pg, sizeof(slab), TYPE_POINTER_ARRAY);
    Iter_Init(&m->it, p);
    status r = Iter_SetByIdx(&m->it, 0, (void *)pg);
    m->it.type.state = ((m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET);
    return r;
}

MemCh *MemCh_OnPage(){
    MemPage *sl = (MemPage *)MemPage_Make(NULL, 0);
    MemCh *m = (MemCh *)MemPage_Alloc(sl, sizeof(MemCh));
    MemCh_Setup(m, sl);
    return m;
}

MemCh *MemCh_Make(){
    MemCh *m = MemCh_OnPage();
    return m;
}
