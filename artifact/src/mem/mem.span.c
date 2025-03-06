#include <external.h>
#include <caneka.h>

static MemCtx nativeM = {
    .type = {TYPE_MEMCTX_NATIVE, 0},
    .itemSize = 0,
    .slotSize = 0,
    .dims = 0,
    .root = NULL,
    .m = NULL,
    .nvalues = 0,
    .max_idx = 0,
    .metrics = {
        .get = -1,
        .set = -1,
        .selected = -1,
    }
};

static size_t cmem = 0;
int MemSlab_Count = 0;

static void *MemSlab_Alloc(MemSlab *sl, word sz){
    sl->remaining -= sz;
    return sl->bytes+sl->remaining; 
}

void *TrackMalloc(size_t sz, cls t){
    void *p = malloc(sz);
    if(p == NULL){
        Fatal("Allocating", t);
        /* not reached */
        return NULL;
    }
    cmem += sz;
    memset(p, 0, sz);
    return p;
}

void TrackFree(void *p, size_t s){
    if(p != NULL){
        free(p);
    }
    cmem -= s;
}

size_t MemCount(){
    return cmem;
}

i64 MemCtx_MemCount(MemCtx *m, i16 level){
    Iter it;
    Iter_Init(&it, m);
    i64 total = 0;
    while((SpanMem_IterNext(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(level == 0 || sl->level == level){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}

MemSlab *MemSlab_Make(MemCtx *m, i16 level){
    void *slab = TrackMalloc(MEM_SLAB_SIZE, TYPE_MEMSLAB);
    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = level,
        .remaining = MEM_SLAB_SIZE,
        .bytes = slab,
    };
    return MemSlab_Attach(m, &sl);
}

void *MemCtx_Alloc(MemCtx *m, size_t sz){
    if(sz > MEM_SLAB_SIZE){
        Fatal("Trying to allocation too much memory at once", TYPE_MEMCTX);
    }

    if(m->type.of == TYPE_MEMCTX_NATIVE){
        return TrackMalloc(sz, TYPE_MEMCTX_NATIVE);
    }

    i16 level = max(m->type.state, 0);
    word _sz = (word)_sz;

    Iter it;
    Iter_Init(&it, m);
    MemSlab *sl = NULL;
    while((SpanMem_IterNext(&it) & END) == 0){
        MemSlab *_sl = (MemSlab *)Iter_Get(&it);
        if((level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemSlab_Make(m, level);
    }

    return  MemSlab_Alloc(sl, _sz);
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

MemCtx *MemCtx_Make(){
    return SpanMem_Make(&nativeM);
}

i64 MemCtx_Total(MemCtx *m, i16 level){
    return MemCtx_MemCount(m, level);
}

status MemCtx_WipeTemp(MemCtx *m, i16 level){
    status r = READY;

    Iter it;
    Iter_Init(&it, m);
    while((SpanMem_IterNext(&it) & END) == 0){
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
    while((SpanMem_IterNext(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(level == 0 || sl->level >= level){
            TrackFree(sl->bytes, sizeof(MemSlab));
            sl->remaining = 0;
            r = SUCCESS;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status MemCtx_Free(MemCtx *m){
    MemCtx_FreeTemp(m, max(m->type.state, 0));
    if(m->type.state <= 0){
        TrackFree(m, sizeof(MemCtx));
    }
    return SUCCESS;
}

MemSlab *MemSlab_Attach(MemCtx *m, MemSlab *sl){
    SpanQuery sq;
    SpanQuery_Setup(&sq, m, SPAN_OP_SET, m->max_idx+1);
    SpanMem_SetFromQ(&sq, (Abstract *)sl);
    if(sq.type.state & SUCCESS){
        return sl;
    }
    return NULL;
}

/* utils */
void *MemCtx_GetSlab(MemCtx *m, void *addr){
    Iter it;
    Iter_Init(&it, m);
    while((SpanMem_IterNext(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        void *start = (void *)sl->bytes;
        void *end = sl->bytes + MEM_SLAB_SIZE;
        if((void *)(sl->bytes) <= addr && addr < end){
            return sl;
        }
    }
    return NULL;
}
