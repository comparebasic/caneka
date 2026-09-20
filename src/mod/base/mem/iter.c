#include <external.h>
#include "base_module.h"

Iter IT;

static inline Slab *Iter_newSlab(MemCh *m, Span *p){
    i16 level = m->level;
    m->level = p->memLevel;
    Slab *sl = NULL;

    if(m->type.state & SPAN_QUEUE){
        Slate *slate = Slate_Make(m);
        sl = &slate->slab;
    }else{
        sl = (Slab *)Bytes_Alloc((m), sizeof(Slab), TYPE_POINTER_ARRAY);
    }

    m->level = level;
    return sl;
}

static status Iter_Query(Iter *it){
    it->type.state &= ~(NOOP|MORE|TAIL);
    MemCh *m = it->p->m;
    Span *p = it->p;

    if(it->type.state & ITER_ADD){
        it->idx = it->p->maxIdx+1;
        it->type.state &= ~END;
    }

    boolean fill = ((it->type.state & (ITER_SET|ITER_RESERVE|ITER_ADD)) != 0);
    if(!fill){
        it->value = NULL;
    }

    if(it->idx > p->size){
        if(!fill){
            it->type.state |= NOOP;
            return it->type.state;
        }

        i8 dims = p->range.range;
        i64 size = p->size;
        while(((size *= SPAN_STRIDE)-1) < it->idx){
            if(dims < 0){
                void *ar[] = {
                    I64_Wrapped(m, SPAN_STRIDE),
                    NULL
                };
                Fatal(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Span unable to grow to that many values: greater than $^255", ar);
            }
            dims++;
        }

        Slab *prev = NULL;
        Slab *shelf = NULL;
        Slab *sl = NULL;
        while(p->range.range < dims){
            sl = (Slab *)Iter_newSlab(m, p);
            if(prev == NULL){
                shelf = it->p->root;
                it->p->root = sl;
            }else{
                *(prev[0]) = sl;
            }

            prev = sl;
            p->range.range++;
        }

        *(prev[0]) = sl;
        p->size = size;
        p->range.range = dims;
    }

    i8 dim = (i8)p->range.range;
    if(it->range.range != p->range.range){
        it->stack = (Slab **)MemCh_Alloc(m, SizeW(Slab **)*(p->range.range*2));
        it->localIdx = (i8 *)MemCh_Alloc(m, SizeW(i8)*(p->range.range*2));
        it->range.range = p->range.range;
    }

    while(dim >= 0){
        i64 local = (it->idx >> (SPAN_DIM_SHIFT*dim)) & (SPAN_LOCAL_MAX);

        Slab *sl = NULL;
        if(dim == p->range.range){
            it->stack[dim] = p->root;
            it->localIdx[dim] = local;
        }else{
            Slab *parent = it->stack[dim+1];
            Slab *sl = *(parent[it->localIdx[dim+1]]);
            if(sl == NULL){
                if(!fill){
                    it->type.state |= NOOP;
                    break;
                }else{
                    sl = Iter_newSlab(m, p);
                    *(parent[it->localIdx[dim+1]]) = sl;
                }
            }
            it->stack[dim] = sl;
            it->localIdx[dim] = local;
        }

        if(dim == 0){
            void **dptr = sl[local];
            if(!fill){
                if(*dptr == NULL){
                    it->type.state |= NOOP;
                }else{
                    it->value = *dptr;
                }
                break;
            }else{
                if(it->type.state & (ITER_SET|ITER_ADD)){
                    if(*dptr == NULL){
                        p->count++;
                    }
                    *dptr = it->value;
                    if(it->idx > p->maxIdx){
                        p->maxIdx = it->idx;
                    }
                    
                }else if(it->type.state & ITER_REMOVE){
                    if(*dptr != NULL){
                        p->count--;
                    }
                    *dptr = NULL;
                    if(it->idx == p->maxIdx){
                        p->maxIdx--;
                    }
                }
            }
        }
        dim--;
    }

    if(it->idx == p->maxIdx){
        it->type.state |= TAIL;
    }else{
        it->type.state &= ~TAIL;
    }

    return it->type.state;
}

void Iter_AddSpan(Iter *it, Span *p){
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Next(&it2) & END) == 0){
        Iter_Add(it, it2.value);
    }
}

void Iter_AddSpanRev(Iter *it, Span *p){
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Prev(&it2) & END) == 0){
        Iter_Add(it, it2.value);
    }
}

status Iter_Incr(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->range.range;
    i64 idx = it->idx;
    Slab *sl;
    Slab *parent;

    it->value = NULL;

    i8 incr = 1;
    if(it->type.state & ITER_REVERSE){
        incr = -1;
    }
    i64 factor = incr;

    if(it->p == NULL || it->p->count == 0){
        it->type.state |= END; 
        return it->type.state;
    }
    if((it->type.state & PROCESS) == 0){
        if(it->stack[0] == NULL){
            word fl = it->type.state;
            it->type.state = (it->type.state & (NORMAL_FLAGS|ITER_REVERSE)) | ITER_GET;
            Iter_Query(it);
            it->type.state = fl;
        }
    }else{
        if(topDim == 0){
            if(((incr > 0) && (it->localIdx[dim] + incr) < SPAN_STRIDE || (it->localIdx[dim] + incr) >= 0)){
                it->localIdx[dim] += incr;
                it->stack[0] += incr;
            }
            idx += factor;
            it->value = *((void **)it->stack[dim]);
        }else{
            i16 guard = 0;
            while(it->value == NULL && dim <= topDim && ((incr > 0) && idx <= it->p->maxIdx || idx >= 0)){
                Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
                if((incr > 0) && (it->localIdx[dim] + incr) < SPAN_STRIDE || (it->localIdx[dim] + incr) < 0){
                    it->localIdx[dim] -= incr;
                    parent = it->stack[dim+1];
                    it->stack[dim] = (Slab *)parent[it->localIdx[dim]];
                    void **dptr = *(it->stack[dim]);

                    idx += factor;
                    if(dim == 0){
                        it->value = *dptr;
                        if(*dptr != NULL || (it->type.state & ITER_SKIP_NULL) == 0){
                            break;
                        }
                    }else if(*dptr != NULL){
                        idx += factor-1;
                        while(dim-1 >= 0){
                            dim--;
                            factor *= SPAN_STRIDE;
                            it->localIdx[dim] = incr > 0 ? 0 : (SPAN_STRIDE-1);
                            parent = it->stack[dim+incr];
                            it->stack[dim] = (Slab *)parent[it->localIdx[dim]];
                            if(*(it->stack[dim]) == NULL){
                                dim++;
                                factor /= SPAN_STRIDE;
                                break;
                            }else if(dim == 0){
                                if(*(it->stack[dim]) == NULL && (it->type.state & ITER_SKIP_NULL)){
                                    continue;
                                }else{
                                    it->value = *(it->stack[dim]);
                                    break;
                                }
                            }
                        }
                    }
                }else{
                    if(incr > 0){
                        idx -= it->localIdx[dim] * factor;
                        it->localIdx[dim] = 0;
                    }else{
                        idx += it->localIdx[dim] * factor;
                        it->localIdx[dim] = 0;
                    }
                    dim++;
                }
            }
        }
    }

    it->idx = idx;
    if(idx == 0){
        it->type.state |= TAIL;
    }else{
        it->type.state &= ~TAIL;
    }

    if(it->value != NULL){
        it->type.state &= ~NOOP;
    }else{
        it->type.state |= NOOP;
    }

    return it->type.state;
}

status Iter_Next(Iter *it){
    it->type.state &= ITER_REVERSE;
    return Iter_Incr(it);
}

status Iter_Prev(Iter *it){
    it->type.state |= ITER_REVERSE;
    return Iter_Incr(it);
}

status Iter_Remove(Iter *it, i64 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_REMOVE;
    it->value = (void *)NULL;
    return Iter_Query(it);
}

void *Iter_Pop(Iter *it){
    Iter_GoToIdx(it, it->p->maxIdx);
    void *value = it->value;
    Iter_Remove(it, it->idx);
    Iter_Prev(it);
    return value;
}

void Iter_Set(Iter *it, i64 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_SET;
    it->idx = idx;
    it->value = value;
    Iter_Query(it);
}

void Iter_Add(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_ADD;
    it->value = value;
    Iter_Query(it);
}

void Iter_Push(Iter *it, void *value){
    i64 idx = it->idx;
    if(it->type.state & END){
        idx--;
    }
    Iter_Add(it, value);
    Iter_GoToIdx(it, idx);
}

void Iter_GoToIdx(Iter *it, i64 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_GET;
    it->idx = idx;
    Iter_Query(it);
}

void Iter_Reset(Iter *it){
    it->type.state &= FOCUS;
    it->idx = 0;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    it->type.of = TYPE_ITER;
    it->p = p;
    it->range.range = -1;
}

Iter *Iter_Make(MemCh *m, Span *p){
    Iter *it = MemCh_Alloc(m, sizeof(Iter));
    if(p != NULL){
        Iter_Init(it, p);
    }
    return it;
}
