#include <external.h>
#include <caneka.h>

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

    int idx = Span_NextIdx(m);
    Span_Set(m, idx, (Abstract *)sl);
    MemChapter_Claim(sl);
    return sl;
}

void *MemSlab_Alloc(MemSlab *sl, word sz){
    sl->remaining -= sz;
    return sl->bytes+((size_t)sl->remaining); 
}


i64 MemCtx_MemCount(MemCtx *m, i16 level){
    Iter it;
    Iter_Init(&it, m);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(level == 0 || sl->level == level){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}


void *MemCtx_Alloc(MemCtx *m, size_t sz){
    if(sz > MEM_SLAB_SIZE){
        Fatal("Trying to allocation too much memory at once", TYPE_MEMCTX);
    }

    i16 level = max(m->type.state, 0);
    word _sz = (word)sz;

    Iter it;
    Iter_Init(&it, m);
    MemSlab *sl = NULL;
    while((Iter_Next(&it) & END) == 0){
        MemSlab *_sl = (MemSlab *)Iter_Get(&it);
        if((level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemSlab_Attach(m, level);
    }

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

MemCtx *MemCtx_OnPage(void *page, MemSlab **slp){
    MemSlab _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = page,
    };
    MemSlab *sl = MemSlab_Alloc(&_sl, sizeof(MemSlab));
    memcpy(sl, &_sl, sizeof(MemSlab));
    *slp = sl;

    MemCtx *m = (MemCtx *)MemSlab_Alloc(sl, sizeof(MemCtx));
    m->type.of = TYPE_SPAN;
    m->slotSize = 1;
    m->ptrSlot = 0;
    m->max_idx = m->metrics.available = m->metrics.get = m->metrics.selected = m->metrics.set = -1;
    m->m = m;
    m->root = MemSlab_Alloc(sl, SLOT_BYTE_SIZE*SPAN_STRIDE);

    Span_Set(m, 0, (Abstract *)sl);
    return m;
}

MemCtx *MemCtx_Make(){
    void *bytes = MemChapter_GetBytes();
    MemSlab *sl = NULL;
    MemCtx *m = MemCtx_OnPage(bytes, &sl);
    MemChapter_Claim(sl);
    return m;
}

i64 MemCtx_Total(MemCtx *m, i16 level){
    return MemCtx_MemCount(m, level);
}

status MemCtx_WipeTemp(MemCtx *m, i16 level){
    status r = READY;

    Iter it;
    Iter_Init(&it, m);
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if((level == 0 || sl->level >= level) && sl->remaining < MEM_SLAB_SIZE){
            size_t sz = MemSlab_Taken(sl); 
            memset(sl->bytes+sl->remaining, 0, sz);
            sl->remaining = MEM_SLAB_SIZE;
            r = SUCCESS;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status MemCtx_FreeTemp(MemCtx *m, i16 level){
    status r = READY;

    Iter it;
    Iter_Init(&it, m);
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(level == 0 || sl->level >= level){
            r |= MemChapter_FreeSlab(m, sl);
            r |= Span_Remove(m, it.idx);
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status MemCtx_Free(MemCtx *m){
    return MemCtx_FreeTemp(m, max(m->type.state, 0));
}

/* utils */
void *MemCtx_GetSlab(MemCtx *m, void *addr){
    Iter it;
    Iter_Init(&it, m);
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        void *start = (void *)sl->bytes;
        void *end = sl->bytes + MEM_SLAB_SIZE;
        if((void *)(sl->bytes) <= addr && addr < end){
            return sl;
        }
    }
    return NULL;
}
