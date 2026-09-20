#include <external.h>
#include "base_module.h"

NextSet queueInitialSet = {
    15, 14, 13, 12,
    11, 10, 9, 8, 
    7, 6, 5, 4,
    3, 2, 1, 0
};

i32 _increments[SPAN_MAX_DIMS+2] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {0, 15, 255, 4095, 65535};
i32 _capacity[SPAN_MAX_DIMS+1] = {16, 256, 4096, 65536, 1048576};

static status Iter_Query(Iter *it);

static inline void *Iter_newSlab(MemCh *m, Span *p){
    i16 level = m->level;
    m->level = p->memLevel;

    if(m->type.state & SPAN_QUEUE){
        QueueSlab *sl = MemCh_Alloc(m, SizeW(QueueSlab));
        sl->type.of = TYPE_QUEUE_SLAB;
        memcpy(&sl->set, &queueInitialSet, sizeof(NextSet));
        s->next = &(sl->set[SPAN_LOCAL_MAX]);

        m->level = level;
        return sl;
    }

    m->level = level;
    return Bytes_Alloc((m), sizeof(slab), TYPE_POINTER_ARRAY);
}

static status Iter_Query(Iter *it){
    it->type.state &= ~(SUCCESS|NOOP|MORE|LAST);
    MemCh *m = it->p->m;
    Span *p = it->p;

    if(it->type.state & SPAN_OP_ADD){
        it->idx = it->p->max_idx+1;
        it->type.state &= ~END;
    }

    boolean fill = ((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD)) != 0);
    if(!fill){
        it->value = NULL;
    }

    if(idx > p->size){
        if(!fill){
            it->type.state |= NOOP;
            return it->type.state;
        }

        i8 dims = p->range.range;
        i64 size = p->size;
        while(((size *= SPAN_STRIDE)-1) < idx){
            if(dims > 255){
                void *ar[] = {
                    I64_Wrapped(m, SPAN_STRIDE);
                    NULL
                };
                Fatal(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Span unable to grow to that many values: greater than $^255", ar);
            }
            dims++;
        }

        Slab *prev = NULL;
        Slab *shelf = NULL;
        while(p->range.range < dims){
            Slab *sl = (slab *)Iter_newSlab(m, Span *p);
            if(prev == NULL){
                shelf = it->p->root;
                it->p->root = sl;
            }else{
                *(prev[0]) = new_sl;
            }

            prev = sl;
            p->range.range++;
        }

        *(prev[0]) = shelf_sl;
        p->size = size;
        p->range.range = dims;
    }

    i8 dim = (i8)p->range.range;
    if(it->range.range != p->range.range){
        it->stack = (Span **)MemCh_Alloc(m, SizeW(Slab **)*(p->range.range*2));
        it->stackIdx = MemCh_Alloc(m, SizeW(i8)*(p->range.range*2));
        it->range.range = p->range.range;
    }

    while(dim >= 0){
        i64 local = (it->idx >> (SPAN_DIM_SHIFT*dim)) & (SPAN_LOCAL_MAX);

        Slab *sl = NULL;
        if(dim == p->rang.range){
            it->stack[dim] = p->root;
            it->localIdx[dim] = local;
        }else{
            Slab *parent = it->stack[dim+1]
            Slab *sl = parent[it->localIdx[dim+1]];
            if(sl == NULL){
                if(!fill){
                    it->type.state |= NOOP;
                    break;
                }else{
                    sl = Iter_newSlab(m, p);
                    (*parent[it->localIdx[dim+1]]) = sl;
                }
            }
            *(it->stack[dim]) = sl;
            it->localIdx[dim] = local;
        }

        if(dim == 0){
            void *ptr = sl[local];
            if(!fill){
                if(*ptr == NULL){
                    it->type.state |= NOOP;
                }else{
                    it->value = *ptr;
                }
                break;
            }else{
                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                    if(*ptr == NULL){
                        p->nvalues++;
                    }
                    *ptr = it->value;
                    if(it->idx > p->maxIdx){
                        p->maxIdx = it->idx;
                    }
                    
                }else if(it->type.state & SPAN_OP_REMOVE){
                    if(*ptr != NULL){
                        p->nvalues--;
                    }
                    *ptr = NULL;
                    if(it->idx == p->maxIdx){
                        p->max_idx--;
                    }
                }
            }
        }
        dim--;
    }

    if(it->idx == p->maxIdx){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    return it->type.state;
}

status Iter_AddSpan(Iter *it, Span *p){
    status r = READY;
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Next(&it2) & END) == 0){
        r |= Iter_Add(it, Iter_Get(&it2));
    }
    return r;
}

status Iter_AddSpanRev(Iter *it, Span *p){
    status r = READY;
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Prev(&it2) & END) == 0){
        r |= Iter_Add(it, Iter_Get(&it2));
    }
    return r;
}


status Iter_Incr(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->dims;
    i64 idx = it->idx;
    Slab *sl;

    it->value = NULL;

    i8 incr = 1;
    if(it->type.state & ITER_REVERSE){
        incr = -1;
    }
    i64 factor = inc;

    if(it->p == NULL || it->p->nvalues == 0){
        it->type.state |= END; 
        return it->type.state;
    }
    if((it->type.state & PROCESSING) == 0){
        if(it->stack[0] == NULL){
            word fl = it->type.state;
            it->type.state = (it->type.state & (NORMAL_FLAGS|ITER_REVERSE)) | SPAN_OP_GET;
            Iter_Query(&it);
            it->type.state = fl;
        }
    }else{
        if(topDim == 0){
            if(((incr > 0) && (it->stackIdx[dim] + incr) < SPAN_STRIDE || (it->stackIdx[dim] + incr) >= 0)){
                it->stackIdx[dim] += incr;
                *(it->stack[dim]) += incr;
            }
            idx += factor;
            it->value = *((void **)it->stack[dim]);
        }else{
            i16 guard = 0;
            while(it->value == NULL && dim <= topDim && ((incr > 0) && idx <= it->p->maxIdx || idx >= 0)){
                Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
                if((incr > 0) && (it->stackIdx[dim] + incr) < SPAN_STRIDE || (it->stackIdx[dim] + incr) < 0){
                    it->stackIdx[dim] -= incr;
                    parent = *(it->stack[dim+1]);
                    *(it->stack[dim]) = parent[it->stackIdx[dim]];
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
                            *(it->stackIdx[dim]) = incr > 0 ? 0 : (SPAN_STRIDE-1);
                            Slab *parent = *(it->stack[dim+incr]);
                            it->stack[dim] = parent[it->stackIdx[dim]];
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
                        idx -= it->stackIdx[dim] * factor;
                        it->stackIdx[dim] = 0;
                    }else{
                        idx += it->stackIdx[dim] * factor;
                        it->stackIdx[dim] = 0;
                    }
                    dim++;
                }
            }
        }
    }

    it->idx = idx;
    if(idx == 0){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    if(it->value != NULL){
        it->type.state &= ~NOOP;
    }else{
        it->type.state |= NOOP;
    }

    return it->type.state;
}

/*
status Iter_Next(Iter *it){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    if(it->p == NULL || it->p->nvalues == 0){
        it->type.state |= END;
        return it->type.state;
    }

    i8 dim = 0;
    i8 topDim = it->p->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    it->value = NULL;
    boolean skipNull = (it->type.state & SPAN_OP_ADD) == 0;
    void **ptr = NULL;

    if(it->type.state & SPAN_OP_ADD){
        if(it->idx != it->p->max_idx){
            idx = it->idx = it->p->max_idx;
            Iter_Query(it);
            it->type.state |= PROCESSING;
        }
        it->type.state &= ~END;
    }

    if((it->type.state & END) || (it->type.state & PROCESSING) == 0){
        word fl = it->type.state & ~(END|LAST);
        if(it->type.state & END){
            idx = 0;
        }else if(it->idx >= 0){
            idx = it->idx;
        }
        Iter_ResetStack(it, idx, fl);
        it->type.state |= (fl|PROCESSING);
        Iter_Query(it);
        goto end;
    }else{
        if(topDim == 0){
            if((it->stackIdx[dim]+1) < SPAN_STRIDE){
                it->stackIdx[dim]++;
                ptr = it->stack[dim];
                it->stack[dim] = ptr+1;
            }
            idx += _increments[dim];
            if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                *((void **)it->stack[dim]) = it->value;
                it->p->nvalues++;
            }else{
                it->value = *((void **)it->stack[dim]);
            }
        }else{
            i32 incr = 1;
            i16 guard = 0;
            while(it->value == NULL && dim <= topDim && 
                    idx <= it->p->max_idx){
                Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
                if((it->stackIdx[dim] + incr) < SPAN_STRIDE){
                    it->stackIdx[dim] += incr;

                    if(dim >= topDim){
                        ptr = (void **)it->p->root;
                    }else{
                        ptr = *((void **)it->stack[dim+1]);
                    }

                    if(ptr != NULL){
                        ptr += it->stackIdx[dim];
                    }
                    it->stack[dim] = ptr;
                    idx += _increments[dim];

                    if(dim == 0){
                        if(ptr != NULL){
                            it->value = *ptr;
                        }
                        if(skipNull){
                            continue;
                        }else{
                            goto end;
                        }
                    }else if(ptr != NULL && *ptr != NULL){
                        i32 offset = idx & _modulos[dim];
                        while(dim-1 >= 0){
                            dim--;
                            if(dim == topDim){
                                ptr = (void **)it->p->root;
                            }else{
                                ptr = *((void **)it->stack[dim+1]);
                            }
                            it->stack[dim] = ptr;
                            if(ptr == NULL){
                                dim++;
                                break;
                            }else if(dim == 0){
                                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                                    *ptr = it->value;
                                    it->p->nvalues++;
                                }else if(ptr != NULL){
                                    it->value = *ptr;
                                }else{
                                    it->value = NULL;
                                }
                                if(!skipNull){
                                    goto end;
                                }
                            }
                        }
                    }
                }else{
                    idx -= it->stackIdx[dim] * _increments[dim];
                    it->stackIdx[dim] = 0;
                    dim++;
                }
            }
        }
    }
end:
    if(idx > it->p->max_idx || dim > topDim){
        it->type.state |= END;
    }else if(idx == it->p->max_idx){
        it->type.state |= LAST;
    }

    it->idx = idx;
    if(((it->type.state & SPAN_OP_GET) && it->value != NULL) ||
            it->type.state & (SPAN_OP_RESERVE|SPAN_OP_ADD)){
        it->type.state &= ~NOOP;
        it->type.state |= SUCCESS;
    }else{
        it->type.state |= NOOP;
        it->type.state &= ~SUCCESS;
    }

    return it->type.state;
}
*/

status Iter_Remove(Iter *it){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_REMOVE;
    it->value = (void *)NULL;
    return Iter_Query(it);
}

void *Iter_Pop(Iter *it){
    void *value = Iter_GetByIdx(it, it->p->max_idx);
    Iter_Remove(it);
    Iter_Prev(it);
    return value;
}

status Iter_Set(Iter *it, i32 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->idx = idx;
    it->value = value;
    status r = Iter_Query(it);
    return r;
}

status Iter_Add(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
    it->value = value;
    status r = Iter_Query(it);
    return r;
}

status Iter_Push(Iter *it, void *value){
    i32 idx = it->idx;
    if(it->type.state & END){
        idx--;
    }
    Iter_Add(it, value);
    Iter_GetByIdx(it, idx);

    return it->type.state;
}

status Iter_GoToIdx(Iter *it, i32 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    it->idx = idx;
    return Iter_Query(it);
}

status Iter_Reset(Iter *it){
    it->type.state &= DEBUG;
    it->idx = 0;
    return ZERO;
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
