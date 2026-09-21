#include <external.h>
#include "base_module.h"

i64 Span_Add(Span *p, void *t){
    if(p->range.range == 0 && (p->maxIdx+1) < SPAN_STRIDE){
        p->maxIdx++;
        printf("Slab at %p value at %p diff %lu\n", 
            p->root,
            &(*p->root)[p->maxIdx],
            (void *)&((*p->root)[p->maxIdx]) - (void *)p->root);
        fflush(stdout);
        (*p->root)[p->maxIdx] = t;
        p->count++;
    }else{
        i64 idx = p->maxIdx+1;
        printf("Iter Stuff for %ld\n", idx);
        fflush(stdout);
        Iter_Init(&IT, p);
        Iter_Set(&IT, idx, t);
    }

    return p->maxIdx;
}

void Span_Set(Span *p, i64 idx, void *t){
    if(idx < 0){
        p->type.state |= ERROR;
        return;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        *(p->root[idx]) = t;
        return;
    }

    Iter_Init(&IT, p);
    Iter_Set(&IT, idx, t);
}

void *Span_Get(Span *p, i64 idx){
    if(idx < 0){
        p->type.state |= ERROR;
        return NULL;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        return (*p->root)[idx];
    }

    Iter_Init(&IT, p);
    Iter_GoToIdx(&IT, idx);
    if((IT.type.state & NOOP) == 0){
        return IT.value;
    }

    return NULL;
}

void Span_Remove(Span *p, i64 idx){
    if(idx < 0){
        p->type.state |= ERROR;
        return;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        if(p->root[idx] != NULL){
            (*p->root)[idx] = NULL;
            p->count--;
        }
    }

    Iter_Init(&IT, p);
    Iter_Remove(&IT, idx);
}

Span *Span_Make(MemCh *m){
    Span *p = MemCh_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->m = m;
    p->maxIdx = -1;
    p->root = (Slab *)Bytes_Alloc((m), sizeof(Slab), TYPE_POINTER_ARRAY);
    p->size = SPAN_STRIDE;
    return p;
}
