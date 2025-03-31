#include <external.h>
#include <caneka.h>

status MemCh_Expand(MemCh *m){
    Span *p = m->it.span;
    i32 idx = p->nvalues+1;
    slab *slabs = (slab *)Span_Get(p, m->nextIdx);
    slabs += (MEM_SLAB_SIZE)-sizeof(MemPage)-(sizeof(slab)*m->nextCount);
    i8 dimsNeeded = 1;
    while(_increments[dimsNeeded-1] <= idx){
        dimsNeeded++;
    }
    MemPage *mem_sl = NULL;
    if(dimsNeeded > p->dims){
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;

            new_sl = (slab *)slabs;
            slabs++;

            if(exp_sl == NULL){
                shelf_sl = p->root;
                p->root = new_sl;
            }else{
                void **ptr = (void *)exp_sl;
                *ptr = new_sl;
            }
            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void *)exp_sl;
        *ptr = shelf_sl;
    }

    return SUCCESS;
}

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
        MemPage *_sl = (MemPage *)m->it.value;
        if(_sl != NULL && (level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemPage_Attach(m, level);
    }
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
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level >= level) && sl->remaining < MEM_SLAB_SIZE){
            size_t sz = MemPage_Taken(sl); 
            memset(sl->bytes+sl->remaining, 0, sz);
            sl->remaining = MEM_SLAB_SIZE;
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
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level >= level)){
            if(m->it.idx > 0){
                r |= Span_Remove(m->it.span, m->it.idx);
            }
            r |= MemBook_FreePage(m, sl);
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
        MemPage *sl = Span_Get(m->it.span, 0);
        return MemBook_FreePage(m, sl);
    }
    return r;
}

void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL){
            void *start = (void *)sl->bytes;
            void *end = sl->bytes + MEM_SLAB_SIZE;
            if((void *)(sl->bytes) <= addr && addr < end){
                *idx = m->it.idx;
                return sl;
            }
        }
    }
    Iter_Reset(&m->it);
    *idx = -1;
    return NULL;
}

status MemCh_ReserveSpanExpand(MemCh *m, MemPage *sl, word nextIdx){
    m->nextIdx = nextIdx;
    m->nextCount = m->it.span->dims+1;
    MemPage_Alloc(sl, sizeof(slab)*m->nextCount);
    return SUCCESS;
}

status MemCh_Setup(MemCh *m, MemPage *sl){
    m->type.of = TYPE_MEMCTX;
    Span *p = MemPage_Alloc(sl, sizeof(Span));
    Span_Setup(p);
    p->m = m;
    p->max_idx = -1;
    p->root = MemPage_Alloc(sl, sizeof(slab));
    Iter_Setup(&m->it, p, SPAN_OP_SET, 0);
    m->it.value = (void *)sl;
    status r = Iter_Query(&m->it);
    m->it.type.state = ((m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET);
    return r;
}

MemCh *MemCh_OnPage(){
    MemPage *sl = (MemPage *)MemPage_Make(NULL, 0);
    MemCh *m = (MemCh *)MemPage_Alloc(sl, sizeof(MemCh));
    MemCh_Setup(m, sl);
    MemCh_ReserveSpanExpand(m, sl, 0);
    return m;
}

MemCh *MemCh_Make(){
    MemCh *m = MemCh_OnPage();
    return m;
}

