#include <external.h>
#include <caneka.h>

static MemCtx nativeM = {
    .type = {TYPE_MEMCTX_NATIVE, 0},
    .slotSize = sizeof(MemSlab),
    .ptrSlot = 1,
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
    printf("  -> %hd taking %hd\n", sl->remaining, sz);
    sl->remaining -= sz;
    return sl->bytes+sl->remaining; 
}

static  status MemSlab_Attach(MemCtx *m, i16 level){
    void *bytes = MemChapter_GetSlab(m->chapter, m, level);
    if(bytes == NULL){
        return ERROR;
    }

    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = level;
        .remaining = MEM_SLAB_SIZE;
        .bytes = bytes;
    }

    int idx = Span_NextIdx(m);
    return Span_Set(m, idx, &sl)
}

size_t MemCount(){
    return cmem;
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

    if(m->type.of == TYPE_MEMCTX_NATIVE){
        printf("native nvalues:%d", m->nvalues);
        return TrackMalloc(sz, TYPE_MEMCTX_NATIVE);
    }

    i16 level = max(m->type.state, 0);
    word _sz = (word)sz;

    Iter it;
    Iter_Init(&it, m);
    MemSlab *sl = NULL;
    while((Iter_Next(&it) & END) == 0){
        MemSlab *_sl = (MemSlab *)Iter_Get(&it);
        printf("iter level:%d/%d _sl:%p sz:%ld/%hd remaining:%d\n", level, _sl->level, _sl, sz, _sz, _sl->remaining);
        if((level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        sl = MemSlab_Attach(m, level);
    }else{
        printf("found sl\n");
    }

    printf("nvalues:%d\n", m->nvalues);
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

MemCtx *MemCtx_Make(){
    void *bytes = MemChapter_GetBytes(_chapterGlobal, m, level);

    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = level;
        .remaining = MEM_SLAB_SIZE;
        .bytes = bytes;
    }

    MemCtx *m = (MemCtx *)MemSlab_Alloc(&sl, sizeof(MemCtx*));
    Span_Init(m);

    m->slotSize = sizeof(MemSlab);
    m->ptrSlot = 1;

    int idx = Span_NextIdx(m);
    MemSlab *_sl = (MemSlab *)Span_Set(m, idx, &sl)
    if(_sl == NULL){
        return NULL;
    }
    MemChapter_Claim(m->chapter, _sl);
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
            r |= MemChapter_FreeSlab(MemChapter *cp, MemCtx *m, MemSlab *sl);
            r |= Span_Remove(m, it.idx);
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
    return sl;
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
