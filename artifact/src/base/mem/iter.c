#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS+1] = {1, 16, 256, 4096, 65536, 1048576};

static inline i32 Iter_SetStack(Iter *it, i8 dim, i32 offset){
    Span *p = it->span; 
    void **ptr = NULL;
    void *debug = ptr;
    i32 localIdx = 0;
    i32 increment = _increments[dim];
    localIdx = (offset / increment);
    if(dim == p->dims){
        ptr = (void **)p->root;
        it->stackIdx[dim] = 0;
    }else{
        ptr = it->stack[dim+1]; 
        /*
        printf("setting pointer to %lu\n", (util)ptr);
        */
        it->stackIdx[dim] = localIdx;
    }

    debug = ptr;
    ptr += localIdx;
    it->stack[dim] = ptr;

    if(localIdx > SPAN_STRIDE){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error localIdx larger than span stride _i4", localIdx);
        return -1;
    }

    if(p->type.state & DEBUG){
        printf("set stack op-set:%d for idx:%d dim:%d offset:%d localIdx:%d slab:*%lu value:*%lu\n",
            (i32)((it->type.state & UPPER_FLAGS) == SPAN_OP_SET), it->idx, (i32)dim, offset, localIdx, (util)debug, (util)*ptr);
    }


    if(offset >= 0){
        return offset % increment;
    }

    return 0;
}

status Iter_NextItem(Iter *it){
    while((Iter_Next(it) & END) == 0 && *((void **)it->stack[0]) == NULL){}
    return it->type.state;
}

status Iter_Next(Iter *it){
    if(it->span->type.state & DEBUG){
        printf("Iter_Next:\n");
    }
    i8 dim = 0;
    i8 topDim = it->span->dims;
    i32 debugIdx = it->idx;
    i32 idx = it->idx;
    if(it->type.state & END){
        if(it->span->type.state & DEBUG){
            printf("    Iter_Next - Reset from END\n");
        }
        Iter_Setup(it, it->span, SPAN_OP_GET, 0);
        Span_Query(it);
        idx = 0;
        goto end;
    }else{
        if((it->stackIdx[0]+1) < SPAN_STRIDE){
            if(it->span->type.state & DEBUG){
                printf("    Iter_Next - Single Incr\n");
            }
            it->stackIdx[0]++;
            void **ptr = it->stack[0];
            it->stack[0] = ptr+1;
            idx++;
            it->type.state |= SUCCESS;
            goto end;

        }
        while(dim <= topDim && (it->stackIdx[dim]+1) == SPAN_STRIDE){
            if(it->span->type.state & DEBUG){
                printf("    \x1b[36mIter_Next - Bulk Incr (dim%d)\x1b[0m\n", (i32)dim);
            }
            idx -= it->stackIdx[dim] * _increments[dim];
            it->stackIdx[dim] = 0;
            dim++;
            void **ptr = it->stack[dim];
            it->stack[dim] = ptr+1;
            idx += _increments[dim];
        }

        while(dim >= 0){
            Iter_SetStack(it, dim, -1);
            dim--;
        }
    }
end:
    if(idx > it->span->max_idx){
        it->type.state |= END;
        if(it->span->type.state & DEBUG){
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
    printf("Iter_Get %lu\n", (util)it->stack[0]);
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
