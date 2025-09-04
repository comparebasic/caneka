#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {0, 15, 255, 4095, 65535, 1048575};
i32 _capacity[SPAN_MAX_DIMS+1] = {16, 256, 4096, 65536, 1048576, 16777216};

static status _Iter_QueryPage(Iter *it, MemPage *pg);

static inline i32 Iter_SetStack(MemCh *m, MemPage *pg, Iter *it, i8 dim, i32 offset){
    Span *p = it->p; 
    void **ptr = NULL;
    void *debug = NULL;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);

    if(localIdx > SPAN_STRIDE){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(m, localIdx), 
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, 
            "Error localIdx larger than span stride _i4", args);
        return -1;
    }

    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        if(it->stack[dim+1] == NULL){
            Fatal(FUNCNAME, FILENAME, LINENUMBER, 
                "Error expected ptr to span in SetStack", NULL);
        }
        ptr = *((void **)it->stack[dim+1]); 
        it->stackIdx[dim] = localIdx;
    }

    ptr += localIdx;
    it->stack[dim] = ptr;
    it->stackIdx[dim] = localIdx;
    if(dim > 0 && *ptr == NULL){
        word fl = (SPAN_OP_SET|SPAN_OP_RESIZE|SPAN_OP_RESERVE|SPAN_OP_ADD);
        if((it->type.state & fl) == 0){
            it->type.state |= FLAG_ITER_CONTINUE;
            return 0;
        }
        if(pg != NULL){
            *ptr = (slab *)BytesPage_Alloc(pg, sizeof(slab));
        }else{
            *ptr = (slab *)Bytes_Alloc((m), sizeof(slab));
        }
    }

    return offset & _modulos[dim];
}

static status Iter_AddWithGaps(Iter *it, MemPage *pg){
    Abstract *value = it->value;
    i32 idx = it->idx;
    i8 dimP = it->p->dims+1;
    Iter prevIt;
    i32 prevIdx = -1;

    if((it->p->max_idx & _modulos[dimP]) == _modulos[dimP]){
        it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_RESERVE;
        it->idx = it->p->max_idx+1;
        it->value = NULL;
        _Iter_QueryPage(it, pg);
        dimP = it->p->dims+1;
    }

    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    _Iter_QueryPage(it, pg);
    i8 dim = 0;
    void *sl = NULL;
    void **last = NULL;
    void **ptr = NULL;
    i64 openSlots = -1;
    if(it->type.state & SUCCESS){
        it->type.state &= ~SUCCESS;

        if(it->p->dims == 0){
            i32 localIdx = it->stackIdx[dim];
            ptr = it->stack[dim];
            last = ptr+((_capacity[0]-1)-localIdx);
            openSlots = (it->p->max_idx+1) - it->idx;
        }else{
            while((it->type.state & SUCCESS) == 0){
                i32 localIdx = it->stackIdx[dim];
                ptr = it->stack[dim];
                last = ptr+((_capacity[0]-1)-localIdx);
                while(*last == NULL){
                    last--;
                    if((openSlots = last-ptr) == 0){
                        break;
                    }
                }

                if(openSlots > 0){
                    break;
                }
                if(dim+1 > it->p->dims){
                    break;
                }
                dim++;
            }
        }

        if(openSlots >= 0){
            memmove(ptr+1, ptr, (openSlots+1)*sizeof(void *));
            *ptr = NULL;
            it->p->max_idx += _increments[dim];
            prevIdx = it->idx + _increments[dim];
            it->type.state |= SUCCESS;
        }
    }

    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->value = value;
    it->idx = idx;
    _Iter_QueryPage(it, pg);
    if(prevIdx != -1){
        Iter_Setup(&prevIt, it->p, SPAN_OP_GET, prevIdx);
        _Iter_QueryPage(&prevIt, pg);
        while(dim > 0 && (it->type.state & SUCCESS)){
            sl = *((void **)prevIt.stack[dim]);
            openSlots = it->stackIdx[dim-1];
            if(sl != NULL && openSlots > 0){
                void *destSl = *((void **)it->stack[dim]);
                memcpy(destSl, sl, (openSlots)*sizeof(void *));
                memset(sl, 0, (openSlots)*sizeof(void *));
            }
            dim--;
        }
    }
    it->type.state = (it->type.state & PROCESSING|SPAN_OP_SET);
    return it->type.state;
}

static status _Iter_QueryPage(Iter *it, MemPage *pg){
    it->type.state &= ~(SUCCESS|NOOP);
    MemCh *m = it->p->m;

    if(it->type.state & SPAN_OP_ADD){
        if(it->type.state & FLAG_ITER_CONTINUE){
            return Iter_AddWithGaps(it, pg);
        }
        it->idx = it->p->max_idx+1;
    }

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= it->idx){
        dimsNeeded++;
    }

    Span *p = it->p;
    if(dimsNeeded > p->dims){
        if((it->type.state &
                (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD|SPAN_OP_RESIZE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            if(pg != NULL){
                new_sl = (slab *)BytesPage_Alloc(pg, sizeof(slab));
            }else{
                new_sl = (slab *)Bytes_Alloc((m), sizeof(slab));
            }

            if(exp_sl == NULL){
                shelf_sl = it->p->root;
                it->p->root = new_sl;
            }else{
                void **ptr = (void **)exp_sl;
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void **)exp_sl;
        *ptr = shelf_sl;
    }

    i8 dim = p->dims;
    i32 offset = it->idx;
    void **ptr = NULL;
    while(dim >= 0){
        offset = Iter_SetStack(p->m, pg, it, dim, offset);
        if(it->type.state & NOOP){
            break;
        }
        if(dim == 0){
            if(it->type.state & (SPAN_OP_SET|SPAN_OP_REMOVE|SPAN_OP_ADD)){
                ptr = (void **)it->stack[0];
                it->type.state |= SUCCESS;
                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                    *ptr = it->value;
                    it->metrics.set = it->idx;
                    p->nvalues++;
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
                ptr = (void **)it->stack[dim];
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

    if(it->idx == p->max_idx){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    return it->type.state;
}


static status Iter_Query(Iter *it){
    return _Iter_QueryPage(it, NULL);
}

static status _Iter_Prev(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    it->value = NULL;
    boolean skipNull = TRUE;
    void **ptr = NULL;

    if((it->type.state & SPAN_OP_GET) == 0){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Iter_Prev can only use Get not Set or Add", NULL);
        return ERROR;
    }

    if(it->p == NULL || it->p->nvalues == 0){
        it->type.state |= END; 
        goto end;
    }

    if((it->type.state & END) || (it->type.state & PROCESSING) == 0){
        idx = it->idx = it->p->max_idx;
        it->type.state &= ~(END|LAST);
        it->type.state |= PROCESSING;

        word prev = it->type.state & SPAN_OP_REMOVE;
        it->type.state &= ~SPAN_OP_REMOVE;
        Iter_Query(it);
        it->type.state |= prev;
        goto end;
    }else{
        if(it->idx == it->p->max_idx && (it->type.state & SPAN_OP_REMOVE)){
            it->p->nvalues--;
            it->p->max_idx--;
            if(it->stack[0] != NULL){
                ptr = (void **)it->stack[0];
                *ptr = NULL;
            }
        }

        if(it->idx == 0){
            it->type.state |= END;
            goto end;
        }
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
            while(it->value == NULL && dim <= topDim && 
                    idx >= 0){
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

status Iter_Set(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->value = value;
    return Iter_Query(it);
}

status Iter_RemoveByIdx(Iter *it, i32 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_REMOVE;
    it->value = (void *)NULL;
    it->idx = idx;
    return Iter_Query(it);
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

    if((it->type.state & END) || !(it->type.state & PROCESSING)){
        word fl = it->type.state & ~(END|LAST);
        if(it->type.state & END){
            idx = 0;
        }else if(it->idx >= 0){
            idx = it->idx;
        }
        Iter_Setup(it, it->p, fl, idx);
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
            while(it->value == NULL && dim <= topDim && 
                    idx <= it->p->max_idx){
                if((it->stackIdx[dim]+ incr) < SPAN_STRIDE){
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
                        if(ptr != NULL && 
                            (it->maskFlags == ZERO || 
                                ((Abstract *)*ptr)->type.state & it->maskFlags) ||
                            (it->filterFlags == ZERO || 
                                (((Abstract *)*ptr)->type.state & it->filterFlags) == 0)
                            ){
                            it->value = *ptr;
                        }
                        if(skipNull){
                            continue;
                        }else{
                            goto end;
                        }
                    }else if(*ptr != NULL){
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
                                }else if(ptr != NULL &&
                                    (it->maskFlags == ZERO || 
                                        ((Abstract *)*ptr)->type.state & it->maskFlags) ||
                                    (it->filterFlags == ZERO || 
                                        (((Abstract *)*ptr)->type.state & it->filterFlags) == 0)
                                ){
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
    if(idx > it->p->max_idx){
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

status Iter_Pop(Iter *it){
    it->type.state = ((it->type.state & NORMAL_FLAGS) & ~(LAST|END)) | 
        (SPAN_OP_GET|SPAN_OP_REMOVE|FLAG_ITER_REVERSE|PROCESSING);
    _Iter_Prev(it);
    return it->type.state;
}

status Iter_PrevRemove(Iter *it){
    it->type.state = ((it->type.state & NORMAL_FLAGS) & ~(LAST|END)) | 
        (SPAN_OP_GET|SPAN_OP_REMOVE|FLAG_ITER_REVERSE);
    _Iter_Prev(it);
    return it->type.state;
}

status Iter_GoToIdx(Iter *it, i32 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    it->idx = idx;
    return Iter_Query(it);
}

status Iter_SetByIdx(Iter *it, i32 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->idx = idx;
    it->value = value;
    status r = Iter_Query(it);
    it->value = NULL;
    return r;
}

status Iter_ExpandTo(Iter *it, i32 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | (SPAN_OP_RESERVE|SPAN_OP_SET);
    it->idx = idx;
    it->value = NULL;
    return Iter_Query(it);
}

status Iter_Push(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
    it->idx = it->p->max_idx;
    it->value = value;
    status r = Iter_Query(it);
    it->type.state |= PROCESSING;
    return r;
}

status Iter_Add(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
    it->idx = it->p->max_idx;
    it->value = value;
    status r = Iter_Query(it);
    it->value = NULL;
    return r;
}

status Iter_Insert(Iter *it, i32 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | (SPAN_OP_ADD|FLAG_ITER_CONTINUE);
    it->idx = idx;
    it->value = value;
    status r = Iter_Query(it);
    it->value = NULL;
    return r;
}

void *Iter_Current(Iter *it){
    if(it->idx < 0){
        return NULL;
    }
    it->type.state &= ~(SUCCESS|NOOP);
    void **ptr = (void **)it->stack[0];
    if(ptr != NULL){
        it->value = *ptr;
        it->type.state |= SUCCESS;
    }else{
        it->value = NULL;
        it->type.state |= NOOP;
    }
    return it->value;
}

void *Iter_GetByIdx(Iter *it, i32 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    it->idx = idx;
    status r = Iter_Query(it);
    if(it->type.state & SUCCESS){
        return it->value;
    }
    return NULL;
}

void *Iter_Get(Iter *it){
    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    return it->value;
}

status Iter_Reset(Iter *it){
    it->type.state &= DEBUG;
    it->idx = 0;
    return SUCCESS;
}

status Iter_Prev(Iter *it){
    it->type.state = (it->type.state & NORMAL_FLAGS) | (SPAN_OP_GET|FLAG_ITER_REVERSE);
    return _Iter_Prev(it);
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
    Iter_Setup(it, p, SPAN_OP_GET, 0);
    return it;
}
