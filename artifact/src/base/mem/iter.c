#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};
i32 _modulos[SPAN_MAX_DIMS+1] = {15, 255, 4095, 65535, 1048575, 16777215};

static inline i32 Iter_SetStack(Iter *it, i8 dim, i32 offset){
    Span *p = it->span; 
    void **ptr = NULL;
    void *debug = NULL;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);

    if(localIdx > SPAN_STRIDE){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "Error localIdx larger than span stride _i4", localIdx);
        return -1;
    }

    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        if(it->stack[dim+1] == NULL){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
                "Error expected ptr to span in SetStack");
        }
        ptr = *((void **)it->stack[dim+1]); 
        it->stackIdx[dim] = localIdx;
    }

    debug = ptr;
    ptr += localIdx;
    it->stack[dim] = ptr;
    it->stackIdx[dim] = localIdx;
    if(dim > 0 && *ptr == NULL){
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESIZE)) == 0){
            it->type.state |= NOOP;
            return 0;
        }
        *ptr = (slab *)MemCh_Alloc((m), sizeof(slab));
        memset(*ptr, 0, sizeof(slab));
    }

    /*
    return offset & _modulos[dim];
    */
    return offset % increment;
}

status Iter_NextItem(Iter *it){
    while((Iter_Next(it) & END) == 0 && *((void **)it->stack[0]) == NULL){}
    return it->type.state;
}

status Iter_Next(Iter *it){
    i8 dim = 0;
    i8 topDim = it->span->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    void **ptr = NULL;
    if((it->type.state & END) || !(it->type.state & PROCESSING)){
        word fl = ((it->type.state) & (~END));
        Iter_Setup(it, it->span, SPAN_OP_GET, 0);
        it->type.state |= (fl|PROCESSING);
        Iter_Query(it);
        idx = 0;
        goto end;
    }else{
        while(dim <= topDim){
            if((it->stackIdx[dim]+1) < SPAN_STRIDE){
                it->stackIdx[dim]++;
                ptr = it->stack[dim];
                if(ptr != NULL){
                    it->stack[dim] = ptr+1;
                }
                idx += _increments[dim];
                it->type.state |= SUCCESS;
                break;
            }else{
                idx -= it->stackIdx[dim] * _increments[dim];
                it->stackIdx[dim] = 0;
                dim++;
                continue;
            }
        }

        i32 offset = idx;
        i32 offsetDims = 0;
        while(offsetDims < dim){
            offset = offset % _increments[offsetDims++];
        }

        while(--dim >= 0){
            Iter_SetStack(it, dim, offset);
        }
    }
end:
    if(idx > it->span->max_idx){
        it->type.state |= END;
    }

    it->idx = idx;
    if(it->stack[0] != NULL){
        it->value = *((void **)it->stack[0]);
    }else{
        it->value = NULL;
    }

    return it->type.state;
}

status Iter_Query(Iter *it){
    it->type.state &= ~SUCCESS;
    MemCh *m = it->span->m;
    i32 idx = it->idx;

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= idx){
        dimsNeeded++;
    }
    printf("dimsNeeded:%d\n", (i32)dimsNeeded);

    Span *p = it->span;
    MemSlab *mem_sl = NULL;
    if(dimsNeeded > p->dims){
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
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
    printf("dims:%d\n", (i32)dim);
    i32 offset = idx;
    void **ptr = NULL;
    while(dim >= 0){
        offset = Iter_SetStack(it, dim, offset);
        if(it->type.state & NOOP){
            break;
        }
        if(dim == 0){
            if(it->type.state & (SPAN_OP_SET|SPAN_OP_REMOVE)){
                ptr = (void **)it->stack[dim];
                *ptr = it->value;
                it->type.state |= SUCCESS;
                if(it->type.state & SPAN_OP_SET){
                    p->nvalues++;
                    if(it->idx > p->max_idx){
                        p->max_idx = it->idx;
                    }
                }else{
                    p->nvalues--;
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
    it->type.state |= END;
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
