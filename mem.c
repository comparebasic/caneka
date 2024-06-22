#include "external.h"
#include "filestore.h"

static size_t cmem = 0;

static void *trackMalloc(size_t s, type t){
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

size_t MemCount(){
    return cmem;
}

void *MemCtx_Alloc(MemCtx *m, size_t s){
    if(s > MEM_SLAB_SIZE){
        Fatal("Trying to allocation too much memory at once", TYPE_MEMCTX);
    }
    MemSlab *sl = NULL, *last = m->start_sl;
    while(last != NULL){
        if(MemSlab_Available(last) >= s){
            sl = last;
            break;
        }
        last = last->next;
    }
    if(sl == NULL){
        sl = MemSlab_Make(m);
    }

    return MemSlab_Alloc(sl, s); 
}

void *MemCtx_Realloc(MemCtx *m, size_t s, void *orig, size_t origsize){
    void *p = MemCtx_Alloc(m, s);
    memcpy(p, orig, origsize);
    return p; 
}

MemCtx *MemCtx_Make(){
    return (MemCtx *) trackMalloc(sizeof(MemCtx), TYPE_MEMCTX);
}

status MemCtx_Free(MemCtx *m){
    MemSlab *current = m->start_sl, *next = NULL;
    while(current != NULL){
        next = current->next;
        trackFree(current, sizeof(MemSlab)+MEM_SLAB_SIZE);
        current = next;
    }
    trackFree(m, sizeof(MemCtx));

    return SUCCESS;
}

void *MemSlab_GetStart(MemSlab *sl){
    return ((void *)sl)+sizeof(MemSlab);
}

MemSlab *MemSlab_Make(MemCtx *m){
    MemSlab *sl = (MemSlab *) trackMalloc(sizeof(MemSlab)+MEM_SLAB_SIZE, TYPE_MEMSLAB);
    sl->addr = MemSlab_GetStart(sl);
    if(m->start_sl == NULL){
        m->start_sl = sl;
    }else{
        MemSlab *last = m->start_sl;
        while(last != NULL && last->next != NULL){
            last = last->next;
        }
        last->next = sl;
    }
    return sl;
}

size_t MemSlab_Available(MemSlab *sl){
    return MEM_SLAB_SIZE - (sl->addr - MemSlab_GetStart(sl));
}

void *MemSlab_Alloc(MemSlab *sl, size_t s){
    if((MemSlab_Available(sl) - s) < 0){
        Fatal("Trying to allocation (slab) too much memory at once", TYPE_MEMSLAB);
    }
    void *p = sl->addr; 
    sl->addr += s;

    return p;
}
