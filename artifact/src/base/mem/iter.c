#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};

static inline i32 Iter_SetStack(Iter *it, i8 dim, i32 offset){
    Span *p = it->span; 
    void **ptr = NULL;
    void *debug = NULL;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);
    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        if(it->stack[dim+1] == NULL){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error expected ptr to span in SetStack");
        }
        ptr = *((void **)it->stack[dim+1]); 
        it->stackIdx[dim] = localIdx;
    }

    debug = ptr;
    ptr += localIdx;
    it->stack[dim] = ptr;
    it->stackIdx[dim] = localIdx;

    if(localIdx > SPAN_STRIDE){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error localIdx larger than span stride _i4", localIdx);
        return -1;
    }

    if(p->type.state & DEBUG){
        printf("set stack op-set:%d for idx:%d dim:%d offset:%d localIdx:%d delta:%lu slab:*%lu ptr:%lu, value:*%lu\n",
            (i32)((it->type.state & UPPER_FLAGS) == SPAN_OP_SET), it->idx, (i32)dim, offset, localIdx, (util)((void *)ptr-debug)/8, (util)debug, (util)ptr, ptr != NULL ? (util)*ptr: 0);
    }

    return offset % increment;
}

status Iter_NextItem(Iter *it){
    while((Iter_Next(it) & END) == 0 && *((void **)it->stack[0]) == NULL){}
    return it->type.state;
}

status Iter_Next(Iter *it){
    if(it->type.state & DEBUG){
        printf("Iter_Next:\n");
    }
    i8 dim = 0;
    i8 topDim = it->span->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    void **ptr = NULL;
    if((it->type.state & END) || !(it->type.state & PROCESSING)){
        if(it->type.state & DEBUG){
            printf("    Iter_Next - Reset from END\n");
        }
        word fl = ((it->type.state) & (~END));
        Iter_Setup(it, it->span, SPAN_OP_GET, 0);
        it->type.state |= (fl|PROCESSING);
        Span_Query(it);
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
                if(dim > 0 && (1 || it->type.state & DEBUG)){
                    printf("    \x1b[36mIter_Next - Bulk Incr (dim%d) to %d *%lu\x1b[0m\n", (i32)dim, it->stackIdx[dim], (util)it->stack[dim]);
                    printf("\x1b[35m\n");
                    Iter_Print(NULL, NULL, 0, (Abstract *)it, 0, TRUE);
                    printf("\x1b[0m\n");
                }
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
        while(offsetDims++ < dim){
            offset = offset % _increments[offsetDims++];
        }

        while(--dim >= 0){
            Iter_SetStack(it, dim, offset);
        }
    }
end:
    if(idx > it->span->max_idx){
        it->type.state |= END;
        if(it->type.state & DEBUG){
            printf("    Iter_Next END - from %d to %d - maxIdx:%d\n", debugIdx, idx, it->span->max_idx);
        }
    }else{
        if(it->span->type.state & DEBUG){
            printf("    Iter_Next from %d to %d - maxIdx:%d\n", debugIdx, idx, it->span->max_idx);
        }
    }
    it->idx = idx;
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    return it->stack[0] != NULL ? *((void **)it->stack[0]) : NULL;
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
