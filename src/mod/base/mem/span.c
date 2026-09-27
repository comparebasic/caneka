#include <external.h>
#include "base_module.h"

i64 Span_Add(Span *p, void *t){
    i64 idx = -1;
    if(p->range.range == 0 &&
            (p->root->idx.max+1) < SPAN_STRIDE &&
            (p->type.state & SPAN_DICT) == 0){
        if(p->type.state & SPAN_QUEUED){
            idx = (i64)Slate_Enqueue(p->root, t);
        }else{
            idx = (i64)Slate_Add(p->root, t);
        }
    }else{
        Iter_Init(&IT, p);
        Iter_Add(&IT, t);
        p->type.state &= (IT.type.state & ERROR);
        idx = IT.idx;
    }

    return idx;
}

i64 Span_Set(Span *p, i64 idx, void *t){
    if(idx < 0){
        p->type.state |= ERROR;
        return -1;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        Slate_Insert(p->root, (i8)idx, t);
        p->type.state &= (p->root->type.state & ERROR);
        return idx;
    }

    Iter_Init(&IT, p);
    Iter_Set(&IT, idx, t);
    p->type.state &= (IT.type.state & ERROR);
    return IT.idx;
}

void *Span_Get(Span *p, i64 idx){
    if(idx < 0){
        p->type.state |= ERROR;
        return NULL;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        return p->root->slots[idx];
    }

    Iter_Init(&IT, p);
    Iter_GoToIdx(&IT, idx);
    p->type.state &= (IT.type.state & ERROR);
    if((IT.type.state & NOOP) == 0){
        return IT.value;
    }

    return NULL;
}

i64 Span_Remove(Span *p, i64 idx){
    if(idx < 0){
        p->type.state |= ERROR;
        return -1;
    }

    if(p->range.range == 0 && idx < SPAN_STRIDE){
        if(p->root->slots[idx] != NULL){
            p->root->slots[idx] = NULL;
            p->root->idx.count--;
        }
    }

    Iter_Init(&IT, p);
    Iter_Remove(&IT, idx);
    p->type.state &= (IT.type.state & ERROR);
    return IT.idx;
}

Span *Span_Make(MemCh *m, field16 flags){
    Span *p = MemCh_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->type.state = flags;
    p->m = m;
    p->size = SPAN_STRIDE;
    p->root = Slate_Make(m);
    return p;
}
