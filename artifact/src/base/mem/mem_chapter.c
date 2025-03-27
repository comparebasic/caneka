#include <external.h>
#include <caneka.h>

static inline status MemCh_Expand(MemCh *m){
    Span *p = m->it.span;
    i32 idx = p->nvalues+1;
    slab *slabs = (slab *)Span_Get(p, m->nextIdx);
    slabs += (MEM_SLAB_SIZE)-sizeof(MemSlab)-(sizeof(slab)*m->nextCount);
    i8 dimsNeeded = 1;
    while(_increments[dimsNeeded-1] <= idx){
        dimsNeeded++;
    }
    MemSlab *mem_sl = NULL;
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

status MemCh_ReserveSpanExpand(MemCh *m, MemSlab *sl, word nextIdx){
    m->nextIdx = nextIdx;
    m->nextCount = m->it.span->dims+1;
    MemSlab_Alloc(sl, sizeof(slab)*m->nextCount);
    return SUCCESS;
}

void *MemSlab_Alloc(MemSlab *sl, word sz){
    sl->remaining -= sz;
    return sl->bytes+((size_t)sl->remaining); 
}

MemSlab *MemSlab_Attach(MemCh *m, i16 level){
    void *bytes = MemBook_GetPage(m);
    if(bytes == NULL){
        return NULL;
    }

    MemSlab _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = level,
        .remaining = MEM_SLAB_SIZE,
        .bytes = bytes,
    };
    MemSlab *sl = MemSlab_Alloc(&_sl, sizeof(MemSlab));
    memcpy(sl, &_sl, sizeof(MemSlab));

    i32 idx = m->it.span->max_idx+1;

    if(_increments[m->it.span->dims] <(m->it.span->nvalues+1)){
        MemCh_Expand(m);
        MemCh_ReserveSpanExpand(m, sl, idx);
    }

    Span_Set(m->it.span, idx, (Abstract *)sl);
    printf("Claim Attach\n");
    MemBook_Claim(bytes);
    return sl;
}

i64 MemCh_MemCount(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
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

    MemSlab *sl = NULL;
    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *_sl = (MemSlab *)Iter_Get(&m->it);
        if(_sl != NULL && (level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemSlab_Attach(m, level);
    }
    Iter_Reset(&m->it);

    return MemSlab_Alloc(sl, _sz);
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

status MemCh_Setup(MemCh *m, MemSlab *sl){
    m->type.of = TYPE_MEMCTX;
    Span *p = MemSlab_Alloc(sl, sizeof(Span));
    Span_Setup(p);
    p->m = m;
    p->max_idx = -1;
    p->root = MemSlab_Alloc(sl, sizeof(slab));
    Iter_Init(&m->it, p);
    Span_Set(p, 0, (Abstract *)sl);
    return SUCCESS;
}

MemCh *MemCh_OnPage(void *page){
    MemSlab _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = page,
    };

    MemCh *m = (MemCh *)MemSlab_Alloc(&_sl, sizeof(MemCh));
    MemCh_Setup(m, &_sl);
    MemCh_ReserveSpanExpand(m, &_sl, 0);
    return m;
}

MemCh *MemCh_Make(){
    printf("MemCh_Make\n");
    void *bytes = MemBook_GetPage(NULL);
    printf("MemCh_Make II\n");
    MemCh *m = MemCh_OnPage(bytes);
    printf("MemCh_Make III\n");
    printf("Claim Make\n");
    MemBook_Claim(bytes);
    return m;
}

i64 MemCh_Total(MemCh *m, i16 level){
    return MemCh_MemCount(m, level);
}

status MemCh_WipeTemp(MemCh *m, i16 level){
    status r = READY;

    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
        if(sl != NULL && (level == 0 || sl->level >= level) && sl->remaining < MEM_SLAB_SIZE){
            size_t sz = MemSlab_Taken(sl); 
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
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
        if(sl != NULL && (level == 0 || sl->level >= level)){
            if(m->it.idx > 0){
                r |= Span_Remove(m->it.span, m->it.idx);
            }
            r |= MemBook_FreeSlab(m, sl);
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
        MemSlab *sl = Span_Get(m->it.span, 0);
        return MemBook_FreeSlab(m, sl);
    }
    return r;
}

/* utils */
void *MemCh_GetSlab(MemCh *m, void *addr, i32 *idx){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
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
