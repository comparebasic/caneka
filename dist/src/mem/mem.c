#include <external.h>
#include <caneka.h>

static size_t cmem = 0;

static void *trackMalloc(size_t s, cls t){
    void *p = malloc(s);
    if(p == NULL){
        Fatal("Allocating", t);
        /* not reached */
        return NULL;
    }
    cmem += s;
    memset(p, 0, s);
    return p;
}

static void trackFree(void *p, size_t s){
    free(p);
    cmem -= s;
}

static size_t MemSlab_Available(MemSlab *sl){
    return MEM_SLAB_SIZE - MemSlab_Taken(sl);
}

static void *MemSlab_Alloc(MemSlab *sl, size_t s){
    void *p = sl->addr; 
    sl->addr += s;

    return p;
}

i16 MemCtx_TempLevel = 0;

size_t MemCount(){
    return cmem;
}

i64 MemCtx_MemCount(MemCtx *m, i16 level){
    MemSlab *sl = NULL, *last = m->start_sl;
    i64 total = 0;
    while(last != NULL){
        if((level == 0 || last->level == level)){
            total += MemSlab_Taken(last);
        }
        last = last->next;
    }
    return total;
}

MemSlab *MemSlab_Make(MemCtx *m, i16 level){
    size_t sz = sizeof(MemSlab);
    MemSlab *sl = (MemSlab *) trackMalloc(sz, TYPE_MEMSLAB);
    sl->addr = MemSlab_GetStart(sl);
    sl->level = level;
    if(m != NULL){
        return MemSlab_Attach(m, sl);
    }else{
        return sl;
    }
}

void *MemCtx_AllocTemp(MemCtx *m, size_t sz, i16 level){
    if(DEBUG_ALLOC){
        printf("\x1b[%dmAlloc %ld of %p\x1b[0m\n", DEBUG_ALLOC, sz, m);
    }
    if(sz > MEM_SLAB_SIZE){
        Fatal("Trying to allocation too much memory at once", TYPE_MEMCTX);
    }
    MemSlab *sl = NULL, *last = m->start_sl;
    while(last != NULL){
        if((level == 0 || last->level == level) && MemSlab_Available(last) >= sz){
            sl = last;
            break;
        }
        last = last->next;
    }
    if(sl == NULL){
        sl = MemSlab_Make(m, level);
    }

    void *ptr = MemSlab_Alloc(sl, sz);
    m->latest.ptr = ptr;
    m->latest.sz = sz;
    m->latest.slabIdx = sl->idx;
    m->latest.offset = ptr - (void *)sl->bytes;

    return ptr;
}

void *MemCtx_Alloc(MemCtx *m, size_t s){
    i16 level = 0;
    if((m->type.state & MEMCTX_TEMP) != 0){
       level = MemCtx_TempLevel; 
    }
    return MemCtx_AllocTemp(m, s, level);
}

i64 MemCtx_Used(MemCtx *m){
    i64 n = 0;
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        n += (sl->addr - MemSlab_GetStart(sl));
        sl = sl->next;
    }

    return n;
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
    MemCtx *m = (MemCtx *) trackMalloc(sizeof(MemCtx), TYPE_MEMCTX);
    m->type.of = TYPE_MEMCTX;
    return m;
}

i64 MemCtx_Total(MemCtx *m, i16 level){
    MemSlab *last = m->start_sl;
    i64 total = 0;
    while(last != NULL && last->next != NULL){
        printf("last %d\n", last->idx);
        if((level == 0) || last->level == level){
            total += MEM_SLAB_SIZE;
        }
        last = last->next;
    }
    return total;
}

status MemCtx_FreeTemp(MemCtx *m, i16 level){
    MemSlab *current = m->start_sl, *next = NULL;
    while(current != NULL){
        next = current->next;
        if(level == 0 || current->level == level){
            trackFree(current, sizeof(MemSlab));
        }
        current = next;
    }
    return SUCCESS;
}

status MemCtx_Free(MemCtx *m){
    MemCtx_FreeTemp(m, 0);
    trackFree(m, sizeof(MemCtx));
    return SUCCESS;
}

MemSlab *MemSlab_Attach(MemCtx *m, MemSlab *sl){
    if(m->start_sl == NULL){
        m->start_sl = sl;
    }else{
        MemSlab *last = m->start_sl;
        while(last != NULL && last->next != NULL){
            last = last->next;
        }
        last->next = sl;
    }
    if(m->index != NULL){
        Span_Add((Span *)m->index, (Abstract *)sl);
        sl->idx = m->index->metrics.set;
    }else{
        sl->idx = m->count++;
    }
    return sl;

}

/* utils */
void *MemCtx_GetSlab(MemCtx *m, void *addr){
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        void *start = MemSlab_GetStart(sl);
        void *end = start + MEM_SLAB_SIZE;
        if(MemSlab_GetStart(sl) <= addr && addr < end){
            return sl;
        }
        sl = sl->next;
    }
    return NULL;
}
