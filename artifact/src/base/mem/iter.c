#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {0, 15, 255, 4095, 65535, 1048575};
i32 _capacity[SPAN_MAX_DIMS+1] = {16, 256, 4096, 65536, 1048576, 16777216};

static inline i32 Iter_SetStack(MemCh *m, Iter *it, i8 dim, i32 offset){
    Span *p = it->span; 
    void **ptr = NULL;
    void *debug = NULL;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);

    if(localIdx > SPAN_STRIDE){
        void *args[] = {&localIdx, NULL};
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "Error localIdx larger than span stride _i4", args);
        return -1;
    }

    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        if(it->stack[dim+1] == NULL){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
                "Error expected ptr to span in SetStack", NULL);
        }
        ptr = *((void **)it->stack[dim+1]); 
        it->stackIdx[dim] = localIdx;
    }

    ptr += localIdx;
    it->stack[dim] = ptr;
    it->stackIdx[dim] = localIdx;
    if(dim > 0 && *ptr == NULL){
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESIZE|SPAN_OP_ADD)) == 0){
            it->type.state |= CONTINUE;
            return 0;
        }
        *ptr = (slab *)MemCh_Alloc((m), sizeof(slab));
    }

    return offset & _modulos[dim];
}

status Iter_Next(Iter *it){
    i8 dim = 0;
    i8 topDim = it->span->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    it->value = NULL;
    boolean skipNull = (it->type.state & SPAN_OP_ADD) == 0;
    void **ptr = NULL;

    if(it->type.state & SPAN_OP_ADD){
        if(it->idx != it->span->max_idx){
            idx = it->idx = it->span->max_idx;
            Iter_Query(it);
            it->type.state |= PROCESSING;
        }
        it->type.state &= ~END;
    }

    if((it->type.state & END) || !(it->type.state & PROCESSING)){
        word fl = it->type.state & ~(END|FLAG_ITER_LAST);
        idx = 0;
        Iter_Setup(it, it->span, fl, idx);
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
                it->span->nvalues++;
            }else{
                it->value = *((void **)it->stack[dim]);
            }
        }else{
            i32 incr = 1;
            while(it->value == NULL && dim <= topDim && 
                    idx <= it->span->max_idx){
                if((it->stackIdx[dim]+ incr) < SPAN_STRIDE){
                    it->stackIdx[dim] += incr;

                    if(dim == topDim){
                        ptr = (void **)it->span->root;
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
                                ptr = (void **)it->span->root;
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
                                    it->span->nvalues++;
                                }else{
                                    it->value = *ptr;
                                }
                                it->value = *ptr;
                                if(!skipNull){
                                    goto end;
                                }
                            }
                        }
                    }
                    incr = 1;
                }else{
                    idx -= it->stackIdx[dim] * _increments[dim];
                    it->stackIdx[dim] = 0;
                    dim++;
                }
            }
        }
    }
end:
    if(idx > it->span->max_idx){
        it->type.state |= END;
    }else if(idx == it->span->max_idx){
        it->type.state |= FLAG_ITER_LAST;
    }

    it->idx = idx;
    if(((it->type.state & SPAN_OP_GET) && it->value != NULL) || it->type.state & (SPAN_OP_RESERVE|SPAN_OP_ADD)){
        it->type.state &= ~NOOP;
        it->type.state |= SUCCESS;
    }else{
        it->type.state |= NOOP;
        it->type.state &= ~SUCCESS;
    }

    return it->type.state;
}

status Iter_Set(Iter *it, void *value){
    Span *p = it->span;
    void **ptr = (void **)it->span->root;
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
    it->type.state &= ~(SUCCESS|NOOP);
    MemCh *m = it->span->m;
    if(it->type.state & SPAN_OP_ADD){
        it->idx = it->span->max_idx+1;
    }

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= it->idx){
        dimsNeeded++;
    }

    Span *p = it->span;
    if(dimsNeeded > p->dims){
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD|SPAN_OP_RESIZE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            new_sl = (slab *)MemCh_Alloc((m), sizeof(slab));

            if(exp_sl == NULL){
                shelf_sl = it->span->root;
                it->span->root = new_sl;
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
        offset = Iter_SetStack(p->m, it, dim, offset);
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
            }else if(it->type.state & (SPAN_OP_GET)){
                ptr = (void **)it->stack[dim];
                it->value = *ptr;
                it->type.state |= SUCCESS;
            }
        }
        dim--;
    }

    return it->type.state;
}

status Iter_Reset(Iter *it){
    it->type.state = (it->type.state & (UPPER_FLAGS|DEBUG)) | END;
    return SUCCESS;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    return Iter_Setup(it, p, SPAN_OP_GET, 0);
}

void Iter_Setup(Iter *it, Span *p, status op, i32 idx){
    it->type.of = TYPE_SPAN_QUERY;
    it->type.state = op;
    it->span = p;
    it->idx = idx;
    it->metrics.get = it->metrics.set = it->metrics.selected = it->metrics.available = -1;
    return;
}

Iter *Iter_Make(MemCh *m, Span *p){
    Iter *it = MemCh_Alloc(m, sizeof(Iter));
    Iter_Setup(it, p, SPAN_OP_GET, 0);
    return it;
}
