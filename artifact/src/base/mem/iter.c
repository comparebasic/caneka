#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {0, 15, 255, 4095, 65535, 1048575};
i32 _capacity[SPAN_MAX_DIMS+1] = {16, 256, 4096, 65536, 1048576, 16777216};

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
            it->type.state |= CONTINUE;
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

status Iter_Prev(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    it->value = NULL;
    boolean skipNull = (it->type.state & SPAN_OP_ADD) == 0;
    void **ptr = NULL;

    if((it->type.state & SPAN_OP_GET) == 0){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Iter_Prev can only use Get not Set or Add", NULL);
        return ERROR;
    }

    if((it->type.state & END) || !(it->type.state & PROCESSING)){
        word fl = it->type.state & ~(END|FLAG_ITER_LAST);
        idx = it->p->max_idx;
        Iter_Setup(it, it->p, fl, idx);
        it->type.state |= (fl|PROCESSING);
        Iter_Query(it);
        goto end;
    }else{
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
                    idx <= it->p->max_idx){
                if((it->stackIdx[dim] - incr) >= 0){
                    it->stackIdx[dim] -= incr;

                    if(dim == topDim){
                        ptr = (void **)it->p->root;
                    }else{
                        ptr = *((void **)it->stack[dim+1]);
                    }

                    ptr += it->stackIdx[dim];
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
        word fl = it->type.state & ~(END|FLAG_ITER_LAST);
        idx = 0;
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

                    if(dim == topDim){
                        ptr = (void **)it->p->root;
                    }else{
                        ptr = *((void **)it->stack[dim+1]);
                    }

                    ptr += it->stackIdx[dim];
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
                                }else{
                                    it->value = *ptr;
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
        it->type.state |= FLAG_ITER_LAST;
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

status Iter_Set(Iter *it, void *value){
    Span *p = it->p;
    void **ptr = (void **)it->p->root;
    if(p->dims > 0){
        ptr = (void **)*((void **)it->stack[1]); 
    }
    ptr += (it->idx & _modulos[1]);
    *ptr = value;
    p->nvalues++;
    if(it->idx > p->max_idx){
        p->max_idx = it->idx;
    }
    return SUCCESS;
}

status Iter_Query(Iter *it){
    return _Iter_QueryPage(it, NULL);
}

status _Iter_QueryPage(Iter *it, MemPage *pg){
    it->type.state &= ~(SUCCESS|NOOP);
    MemCh *m = it->p->m;
    if(it->type.state & SPAN_OP_ADD){
        it->idx = it->p->max_idx+1;
    }

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= it->idx){
        dimsNeeded++;
    }

    Span *p = it->p;
    if(dimsNeeded > p->dims){
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD|SPAN_OP_RESIZE)) == 0){
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
                ptr = (void **)it->stack[dim];
                *ptr = it->value;
                it->type.state |= SUCCESS;
                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                    p->nvalues++;
                    if(it->idx > p->max_idx){
                        p->max_idx = it->idx;
                    }
                }else if(it->type.state & SPAN_OP_REMOVE){
                    p->nvalues--;
                    if(it->idx == p->max_idx){
                        p->max_idx--;
                    }
                }
            }else if(it->type.state & (SPAN_OP_GET|SPAN_OP_RESERVE)){
                ptr = (void **)it->stack[dim];
                if(ptr != NULL){
                    it->value = *ptr;
                }else{
                    it->value = NULL;
                }
                it->type.state |= SUCCESS;
            }
        }
        dim--;
    }

    if(it->type.state & DEBUG){
        Abstract *args[] = {(Abstract *)it, NULL};
        Out("^c.Iter-Query @^0.\n", args);
    }

    if(it->idx == p->max_idx){
        it->type.state |= FLAG_ITER_LAST;
    }else{
        it->type.state &= ~FLAG_ITER_LAST;
    }

    return it->type.state;
}

status Iter_Reset(Iter *it){
    it->type.state = (it->type.state & (UPPER_FLAGS|DEBUG)) | END;
    it->idx = 0;
    return SUCCESS;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    return Iter_Setup(it, p, SPAN_OP_GET, 0);
}

void Iter_Setup(Iter *it, Span *p, status op, i32 idx){
    it->type.of = TYPE_ITER;
    /*
    it->type.state = (it->type.state & NORMAL_FLAGS) | op;
    */
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
