#include <external.h>
#include "base_module.h"

status Span_Resize(Span *p, i64 size){
    MemCh *m = p->m;
    i8 dims = p->range.range;
    i64 current = p->size;
    while((current-1) < size){
        current *= SPAN_STRIDE;
        if(dims >= DIM_MAX){
            Fatal(m, FUNCNAME, FILENAME, LINENUMBER,
                "Span unable to grow to greater than STRIDE^DIM_MAX", NULL);
        }
        dims++;
    }

    Slate *prev = NULL;
    Slate *shelf = NULL;
    Slate *sl = NULL;
    while(p->range.range < dims){
        sl = Slate_Make(m);
        if(prev == NULL){
            shelf = p->root;
            p->root = sl;
        }else{
            prev->slots[0] = sl;
        }

        prev = sl;
        p->range.range++;
    }

    prev->slots[0] = shelf;
    p->size = current;
    p->range.range = dims;
}

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

        if(idx > p->maxIdx){
            p->maxIdx = idx;
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

        if(idx > p->maxIdx){
            p->maxIdx = idx;
        }
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

            if(idx == p->maxIdx){
                i64 i = idx;
                i--;
                while(i >= 0 && p->root->slots[i] == NULL){
                    i--;
                }
                p->maxIdx = idx;
            }

        }

        return idx;
    }

    Iter_Init(&IT, p);
    Iter_Remove(&IT, idx);
    p->type.state &= (IT.type.state & ERROR);
    return IT.idx;
}

void Span_Init(MemCh *m, Span *p, field16 flags, Slate *root){
    p->type.of = TYPE_SPAN;
    p->type.state = flags;
    p->m = m;
    p->size = SPAN_STRIDE;
    p->root = root;
}

Span *Span_Make(MemCh *m, field16 flags){
    Span *p = MemCh_Alloc(m, sizeof(Span));
    Span_Init(m, p, flags, Slate_Make(m));
    return p;
}
