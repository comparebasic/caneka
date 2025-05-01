#include <external.h>
#include <caneka.h>

i64 MemCh_MemCount(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}

void *MemCh_Alloc(MemCh *m, size_t sz){
    if(sz > MEM_SLAB_SIZE){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Trying to allocation too much memory at once");
    }

    i16 level = max(m->type.range, 0);
    word _sz = (word)sz;

    MemPage *sl = NULL;
    while((Iter_Next(&m->it) & END) == 0){
        if(m->it.span->type.state & DEBUG){
            printf("Looking for mem at %d of %d level %d\n", m->it.idx, m->it.span->nvalues, level);
        }
        MemPage *_sl = (MemPage *)m->it.value;
        if(_sl != NULL && (level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        if(m->it.span->type.state & DEBUG){
            printf("New Page for level%d\n", level);
        }
        sl = MemPage_Make(m, level);
        m->it.type.state = (m->it.type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
        m->it.value = (void *)sl;
        Iter_Query(&m->it);
        if(_increments[m->it.span->dims+1] < (m->it.span->nvalues+1)){
            m->it.value = NULL;
            Iter_Query(&m->it);
        }
    }

    m->it.type.state = (m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    Iter_Reset(&m->it);

    return MemPage_Alloc(sl, _sz);
}

i64 MemCh_Used(MemCh *m){
    return MemCh_MemCount(m, 0);
}

void *MemCh_Realloc(MemCh *m, size_t s, void *orig, size_t origsize){
    if(s > origsize){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Asking to copy more than newly allocated");
        return NULL;
    }
    void *p = MemCh_Alloc(m, s);
    memcpy(p, orig, origsize);
    return p; 
}

i64 MemCh_Total(MemCh *m, i16 level){
    return MemCh_MemCount(m, level);
}

status MemCh_WipeTemp(MemCh *m, i16 level){
    status r = READY;

    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && (level == 0 || pg->level >= level) && pg->remaining < MEM_SLAB_SIZE){
            size_t sz = MemPage_Taken(pg); 
            void *ptr = pg;
            ptr += sizeof(MemPage);
            ptr += pg->remaining;
            memset(ptr, 0, sz);
            pg->remaining = MEM_SLAB_SIZE;
            r = SUCCESS;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    Iter_Reset(&m->it);

    return r;
}

status MemCh_FreeTemp(MemCh *m, i16 level){
    status r = READY;

    Iter_Init(&m->it, m->it.span);
    m->it.type.state |= FLAG_ITER_REVERSE;
    while((Iter_Next(&m->it) & END) == 0){
        if(m->it.idx == 0){
            continue;
        }
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && (level == 0 || pg->level >= level)){
            if(m->it.idx > 0){
                r |= Span_Remove(m->it.span, m->it.idx);
            }
            r |= MemBook_FreePage(m, pg);
        }
    }

    if(r == READY){
        r |= NOOP;
    }
    Iter_Reset(&m->it);

    return r;
}

status MemCh_Free(MemCh *m){
    status r = MemCh_FreeTemp(m, max(m->type.range, 0));
    if(m->type.range == 0){
        MemPage *pg = Span_Get(m->it.span, 0);
        return MemBook_FreePage(m, pg);
    }
    return r;
}

void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL){
            void *start = (void *)pg;
            void *end = (void *)pg;
            end += sizeof(MemPage)+MEM_SLAB_SIZE;
            if(addr >= start && addr <= end){
                *idx = m->it.idx;
                return pg;
            }
        }
    }

    Iter_Reset(&m->it);
    *idx = -1;
    return NULL;
}

status MemCh_Setup(MemCh *m, MemPage *sl){
    m->type.of = TYPE_MEMCTX;
    Span *p = MemPage_Alloc(pg, sizeof(Span));
    Span_Setup(p);
    p->m = m;
    p->max_idx = -1;
    p->root = MemPage_Alloc(pg, sizeof(slab));
    Iter_Setup(&m->it, p, SPAN_OP_SET, 0);
    m->it.value = (void *)pg;
    status r = Iter_Query(&m->it);
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
