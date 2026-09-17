#include <external.h>
#include "base_module.h"

i32 _increments[SPAN_MAX_DIMS+2] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {0, 15, 255, 4095, 65535};
i32 _capacity[SPAN_MAX_DIMS+1] = {16, 256, 4096, 65536, 1048576};

static status Iter_Query(Iter *it);

static inline void *Iter_newSlab(MemCh *m){
    return Bytes_Alloc((m), sizeof(slab), TYPE_POINTER_ARRAY);
}

static inline void *Iter_newSlabForMem(Iter *it){
    return Bytes_AllocOnPage((MemPage *)it->value,
        sizeof(slab), TYPE_POINTER_ARRAY);
}

static inline void Iter_expand(Iter *it, i8 dimsNeeded){
    if((it->type.state &
            (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD)) == 0){
        it->type.state |= NOOP;
        return it->type.state;
    }
    slab *exp_sl = NULL;
    slab *shelf_sl = NULL;
    while(it->p->dims < dimsNeeded){
        Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
        slab *new_sl = NULL;
        if(it->p->nvalues > 0 && it->p->m->it.p == it->p){
            MemPage *pg = it->value;
            i16 level = pg->level;
            pg->level = 0;
            new_sl = (slab *)Iter_newSlabForMem(it);
            pg->level = level;
        }else{
            i16 level = m->level;
            m->level = p->memLevel;
            new_sl = (slab *)Iter_newSlab(m);
            m->level = level;
        }

        if(exp_sl == NULL){
            shelf_sl = it->p->root;
            it->p->root = new_sl;
        }else{
            exp_sl[0] = new_sl;
        }

        exp_sl = new_sl;
        p->dims++;
    }
    exp_sl[0] = shelf_sl;
    it->type.state |= MORE;
}

static inline i32 Iter_SetStack(MemCh *m, Iter *it, i8 dim, i32 offset){
    Span *p = it->p; 
    void **ptr = NULL;
    void *debug = NULL;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);

    if(localIdx > SPAN_STRIDE){
        void *args[] = {
            I32_Wrapped(m, localIdx), 
            NULL
        };
        Fatal(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error localIdx larger than span stride $", args);
        return -1;
    }

    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        if(it->stack[dim+1] == NULL){
            Fatal(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Error expected ptr to span in SetStack", NULL);
        }
        ptr = *((void **)it->stack[dim+1]); 
        it->stackIdx[dim] = localIdx;
    }

    word fl = (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD);
    if((it->p->type.state & fl) == 0 && ptr == NULL){
        it->type.state |= NOOP;
        return 0;
    }

    ptr += localIdx;
    it->stack[dim] = ptr;
    it->stackIdx[dim] = localIdx;
    if(dim > 0 && *ptr == NULL){
        if((it->type.state & fl) == 0){
            it->type.state |= FLAG_ITER_CONTINUE;
            return 0;
        }
        if(p->nvalues > 0 && p->m->it.p == p){
            MemPage *pg = (MemPage *)it->value;
            pg->level = 0;
            *ptr = (slab *)Bytes_AllocOnPage(it->value, sizeof(slab), TYPE_POINTER_ARRAY);
        }else{
            i16 level = m->level;
            m->level = p->memLevel;
            *ptr = (slab *)Bytes_Alloc((m), sizeof(slab), TYPE_POINTER_ARRAY);
            m->level = level;
        }
    }

    return offset & _modulos[dim];
}

static status Iter_Query(Iter *it){
    it->type.state &= ~(SUCCESS|NOOP|MORE|LAST);
    MemCh *m = it->p->m;
    i16 guard = 0;

    if(it->type.state & SPAN_OP_ADD){
        it->idx = it->p->max_idx+1;
        it->type.state &= ~END;
    }

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= it->idx){
        if(++dimsNeeded > SPAN_MAX_DIMS){
            void *args[] = {
                I32_Wrapped(m, it->idx),
                I32_Wrapped(m, it->p->nvalues), 
                NULL
            };
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "idx too large $ for nvalues $", args);
            it->type.state |= ERROR;
            return it->type.state;
        }
    }

    Span *p = it->p;
    if(dimsNeeded > p->dims){
        Iter_expand(it, dimsNeeded);
    }

    i8 dim = p->dims;
    i32 offset = it->idx;
    void **ptr = NULL;
    guard = 0;
    while(dim >= 0){
        Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
        offset = Iter_SetStack(p->m, it, dim, offset);
        if(it->type.state & NOOP){
            break;
        }
        if(dim == 0){
            if(it->type.state & (SPAN_OP_SET|SPAN_OP_REMOVE|SPAN_OP_ADD)){
                ptr = (void **)it->stack[0][it->stackIdx[0]];
                it->type.state |= SUCCESS;
                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                    if(*ptr == NULL){
                        p->nvalues++;
                    }
                    *ptr = it->value;
                    it->metrics.set = it->idx;
                    if(it->idx > p->max_idx){
                        p->max_idx = it->idx;
                    }
                }else if(it->type.state & SPAN_OP_REMOVE){
                    *ptr = NULL;
                    p->nvalues--;
                    if(it->idx == p->max_idx){
                        p->max_idx--;
                    }
                }
            }else if(it->type.state & (SPAN_OP_GET|SPAN_OP_RESERVE)){
                ptr = (void **)it->stack[dim][it->stackIdx[dim]];
                if(ptr != NULL){
                    it->value = *ptr;
                    it->type.state |= SUCCESS;
                    it->metrics.get = it->idx;
                }else{
                    it->value = NULL;
                    it->type.state |= NOOP;
                }
            }
        }
        dim--;
    }

end:
    if(it->idx == p->max_idx){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    return it->type.state;
}

static status Iter_Prev(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    it->value = NULL;
    boolean skipNull = TRUE;
    void **ptr = NULL;

    if((it->type.state & SPAN_OP_GET) == 0){
        Error(it->p->m, FUNCNAME, FILENAME, LINENUMBER,
            "Iter_Prev can only use Get not Set or Add", NULL);
        return ERROR;
    }

    if(it->p == NULL || it->p->nvalues == 0){
        idx = -1;
        it->type.state |= END; 
        goto end;
    }

    if((it->type.state & END) || (it->type.state & PROCESSING) == 0){
        idx = it->idx = it->p->max_idx;
        it->type.state &= ~(END|LAST);
        it->type.state |= PROCESSING;

        word fl = it->type.state & (SPAN_OP_REMOVE|FLAG_ITER_REVERSE);
        it->type.state &= ~(fl);
        Iter_Query(it);
        it->type.state |= fl;

        goto end;
    }else{
        if(topDim == 0){
            if((it->stackIdx[dim]-1) >= 0){
                it->stackIdx[dim]--;
                ptr = it->stack[dim];
                it->stack[dim] = ptr-1;
            }
            idx -= _increments[dim];
            it->value = *((void **)it->stack[dim]);
        }else{
            i32 incr = 1;
            i16 guard = 0;
            while(it->value == NULL && dim <= topDim && 
                    idx >= 0){
                Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
                if((it->stackIdx[dim] - incr) >= 0){
                    it->stackIdx[dim] -= incr;

                    if(dim == topDim){
                        ptr = (void **)it->p->root;
                    }else{
                        ptr = *((void **)it->stack[dim+1]);
                    }

                    ptr += it->stackIdx[dim];
                    it->stack[dim] = ptr;
                    idx -= _increments[dim];
                    if(dim == 0){
                        if(ptr != NULL){
                            it->value = *ptr;
                        }
                        if(skipNull){
                            continue;
                        }else{
                            goto end;
                        }
                    }else if(*ptr != NULL){
                        idx += _increments[dim]-1;
                        i32 offset = idx & _modulos[dim];
                        while(dim-1 >= 0){
                            dim--;
                            if(dim == topDim){
                                ptr = (void **)it->p->root;
                            }else{
                                ptr = *((void **)it->stack[dim+1]);
                            }
                            it->stackIdx[dim] = (SPAN_STRIDE-1);
                            ptr += it->stackIdx[dim];
                            it->stack[dim] = ptr;
                            if(ptr == NULL){
                                dim++;
                                break;
                            }else if(dim == 0){
                                it->value = *ptr;
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
    it->idx = idx;
    if(idx == 0){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    if(((it->type.state & SPAN_OP_GET) && it->value != NULL)){
        it->type.state &= ~NOOP;
        it->type.state |= SUCCESS;
    }else{
        it->type.state |= NOOP;
        it->type.state &= ~SUCCESS;
    }

    return it->type.state;
}

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

void Iter_Start(Iter *it){
    it->type.state &= ~(END|LAST);
    it->type.state |= (PROCESSING|SPAN_OP_GET);

    if(it->type.state & FLAG_ITER_REVERSE){
        it->idx = it->p->max_idx;
    }else{
        it->idx = 0;
    }

    memset(it->stack, 0, sizeof(void *)*SPAN_MAX_DIMS);
    memset(it->stackIdx, 0, sizeof(i32)*SPAN_MAX_DIMS);
    it->value = NULL;

    Iter_Query(it);
}

status Iter_Set(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->value = value;
    return Iter_Query(it);
}

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

status Iter_SetByIdx(Iter *it, i32 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->idx = idx;
    it->value = value;
    status r = Iter_Query(it);
    return r;
}

status Iter_ExpandTo(Iter *it, i32 idx){
    it->type.state = (it->type.state & PROCESSING) | (SPAN_OP_RESERVE|SPAN_OP_SET);
    it->idx = idx;
    it->value = NULL;
    status r = Iter_Query(it);
    it->p->nvalues--;
    return r;
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

void *Iter_GetByIdx(Iter *it, i32 idx){
    status r = Iter_GoToIdx(it, idx);
    if(it->type.state & SUCCESS){
        return it->value;
    }
    return NULL;
}

status Iter_First(Iter *it){
    word flags = NORMAL_FLAGS & ~(END|PROCESSING);
    it->type.state = (it->type.state & flags) | SPAN_OP_GET;
    it->idx = 0;
    status r = Iter_Query(it);
    return it->type.state;
}

void *Iter_GetSelected(Iter *it){
    return Iter_GetByIdx(it, it->metrics.selected);
}

void *Iter_Get(Iter *it){
    return it->value;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    it->type.of = TYPE_ITER;
    it->p = p;
    it->metrics.get = it->metrics.set = it->metrics.selected = it->metrics.available = -1;
    memset(it->stack, 0, sizeof(void *)*SPAN_MAX_DIMS);
    memset(it->stackIdx, 0, sizeof(i32)*SPAN_MAX_DIMS);
    it->value = NULL;
}

void Iter_Restart(Iter *it){
    it->type.state &= ~PROCESSING;
    it->idx = 0;
}

status Iter_Reset(Iter *it){
    it->type.state &= DEBUG;
    it->idx = 0;
    return ZERO;
}

void Iter_ResetStack(Iter *it, i32 idx, status op){
    it->type.state = op;
    it->idx = idx;
    memset(it->stack, 0, sizeof(void *)*SPAN_MAX_DIMS);
    memset(it->stackIdx, 0, sizeof(i32)*SPAN_MAX_DIMS);
    it->value = NULL;
    return;
}

void Iter_Setup(Iter *it, Span *p, status op, i32 idx){
    it->type.of = TYPE_ITER;
    it->type.state = op;
    it->p = p;
    it->idx = idx;
    it->metrics.get = it->metrics.set = it->metrics.selected = it->metrics.available = -1;
    memset(it->stack, 0, sizeof(void *)*SPAN_MAX_DIMS);
    memset(it->stackIdx, 0, sizeof(i32)*SPAN_MAX_DIMS);
    it->value = NULL;
    return;
}

Iter *Iter_Make(MemCh *m, Span *p){
    Iter *it = MemCh_Alloc(m, sizeof(Iter));
    if(p != NULL){
        Iter_Setup(it, p, SPAN_OP_GET, 0);
    }
    return it;
}
