#include <external.h>
#include <caneka.h>

void *MemSlab_Alloc(MemSlab *sl, word sz){
    sl->remaining -= sz;
    /*
    printf("MemSlab_Alloc:%p sz:%hu remaining:%hu\n", sl, sz, sl->remaining);
    */
    return sl->bytes+((size_t)sl->remaining); 
}

MemSlab *MemSlab_Attach(MemCtx *m, i16 level){
    void *bytes = MemChapter_GetBytes();
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

    int idx = m->p.max_idx+1;
    Span_Set(&m->p, idx, (Abstract *)sl);
    MemChapter_Claim(sl);
    return sl;
}

i64 MemCtx_MemCount(MemCtx *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}

void *MemCtx_Alloc(MemCtx *m, size_t sz){
    if(sz > MEM_SLAB_SIZE){
        Fatal("Trying to allocation too much memory at once", TYPE_MEMCTX);
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

i64 MemCtx_Used(MemCtx *m){
    return MemCtx_MemCount(m, 0);
}

void *MemCtx_Realloc(MemCtx *m, size_t s, void *orig, size_t origsize){
    if(s > origsize){
        Fatal("Asking to copy more than newly allocated", TYPE_MEMCTX);
        return NULL;
    }
    void *p = MemCtx_Alloc(m, s);
    memcpy(p, orig, origsize);
    return p; 
}

status MemCtx_Setup(MemCtx *m, MemSlab *sl){
    memcpy(&m->first, sl, sizeof(MemSlab));
    m->type.of = TYPE_MEMCTX;
    Span *p = &m->p;
    Span_Setup(p);
    p->m = m;
    p->root = MemSlab_Alloc(&m->first, sizeof(slot)*SPAN_STRIDE);
    Iter_Init(&m->it, p);
    Span_Set(&m->p, 0, (Abstract *)&m->first);
    return SUCCESS;
}

MemCtx *MemCtx_OnPage(void *page){
    MemSlab _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = page,
    };

    MemCtx *m = (MemCtx *)MemSlab_Alloc(&_sl, sizeof(MemCtx));
    MemCtx_Setup(m, &_sl);
    return m;
}

MemCtx *MemCtx_Make(){
    void *bytes = MemChapter_GetBytes();
    MemCtx *m = MemCtx_OnPage(bytes);
    MemChapter_Claim(&m->first);
    return m;
}

i64 MemCtx_Total(MemCtx *m, i16 level){
    return MemCtx_MemCount(m, level);
}

status MemCtx_WipeTemp(MemCtx *m, i16 level){
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

status MemCtx_FreeTemp(MemCtx *m, i16 level){
    status r = READY;

    Iter_InitReverse(&m->it, &m->p);
    while((Iter_Next(&m->it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&m->it);
        if(sl != NULL && (level == 0 || sl->level >= level)){
            if(m->it.idx > 0){
                r |= Span_Remove(&m->p, m->it.idx);
            }
            r |= MemChapter_FreeSlab(m, sl);
        }
    }

    if(r == READY){
        r |= NOOP;
    }
    Iter_Reset(&m->it);

    return r;
}

status MemCtx_Free(MemCtx *m){
    return MemCtx_FreeTemp(m, max(m->type.range, 0));
}

/* utils */
void *MemCtx_GetSlab(MemCtx *m, void *addr, i32 *idx){
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
