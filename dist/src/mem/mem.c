#include <external.h>
#include <caneka.h>

static size_t cmem = 0;
int MemSlab_Count = 0;

static void *trackMalloc(size_t sz, cls t){
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

static void trackFree(void *p, size_t s){
    free(p);
    cmem -= s;
}

static void *MemSlab_Alloc(MemSlab *sl, size_t s){
    void *p = sl->addr; 
    sl->addr += s;

    return p;
}

size_t MemSlab_Available(MemSlab *sl){
    return MEM_SLAB_SIZE - MemSlab_Taken(sl);
}

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
    sl->type.of = TYPE_MEMSLAB;
    sl->addr = sl->bytes;
    sl->level = level;
    MemSlab_Count++;
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
    return MemCtx_AllocTemp(m, s, max(m->type.range, 0));
}

i64 MemCtx_Used(MemCtx *m){
    i64 n = 0;
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        n += (sl->addr - (void *)(sl->bytes));
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
    while(last != NULL){
        if(level == 0 || last->level == level){
            total += MEM_SLAB_SIZE;
        }
        last = last->next;
    }
    return total;
}

status MemCtx_FreeTemp(MemCtx *m, i16 level){
    MemSlab *current = m->start_sl;
    MemSlab *next = NULL;
    MemSlab *latest = NULL;
    while(current != NULL){
        next = current->next;
        if(level == 0 || current->level >= level){
            trackFree(current, sizeof(MemSlab));
            MemSlab_Count--;
            if(latest == NULL){
                m->start_sl = NULL; 
            }else{
                latest->next = next;
            }
            current = next;
        }else{
            if(m->start_sl == NULL){
                m->start_sl = current;
            }
            latest = current;
            current = next;
        }
    }
    return SUCCESS;
}

status MemCtx_Free(MemCtx *m){
    MemCtx_FreeTemp(m, max(m->type.range, 0));
    if(m->type.range <= 0){
        trackFree(m, sizeof(MemCtx));
    }
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
    sl->idx = m->count++;
    return sl;

}

/* utils */
void *MemCtx_GetSlab(MemCtx *m, void *addr){
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        void *start = (void *)sl->bytes;
        void *end = start + MEM_SLAB_SIZE;
        if((void *)(sl->bytes) <= addr && addr < end){
            return sl;
        }
        sl = sl->next;
    }
    return NULL;
}
