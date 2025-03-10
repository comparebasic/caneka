#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    i8 dim = 0;
    SpanQuery *sq = &it->sq;
    SpanState *st = NULL;
    void **start = NULL;
    void **ptr = NULL;
    void **last = NULL;
    int max_idx = it->values->max_idx;
    if(it->values == NULL || it->values->nvalues == 0){
        it->type.state |= END;
        return it->type.state;
    }
    if((it->type.state & END) != 0){
        it->idx = 0;
        SpanQuery_Setup(&it->sq, it->values, SPAN_OP_GET, 0);
        Span_Query(&it->sq);
        it->type.state &= ~(END|FLAG_ITER_LAST);

        st = &sq->stack[dim];
        start = (void **)st->slab;
        ptr = start+(st->localIdx);
        if(*ptr != 0){
            return it->type.state;
        }
    }
    if((it->idx+1) > it->values->max_idx){
        it->type.state |= END;
        return it->type.state;
    }else{
        boolean loop = TRUE;
        while(dim <= sq->dims && loop){
            st = &sq->stack[dim];
            start = (void **)st->slab;
            ptr = start+(st->localIdx);
            last = start+(SPAN_STRIDE-1);
            start = ptr;
            while(ptr < last){
                if(sq->idx >= max_idx){
                    loop = FALSE;
                    break;
                }
                st->localIdx++;
                sq->idx += st->increment;
                if(*(++ptr) != 0){
                    break;
                }
                /*
                printf("dim %d nvalues:%d max_idx:%d localIdx: %d %p %p\n", 
                    dim, it->values->nvalues, it->values->max_idx,
                    st->localIdx, (void *)*ptr , ptr); 
                    */
            }
            if(*ptr != 0 && ptr != start){
                break;
            }else{
                sq->idx -= st->localIdx;
                st->localIdx = 0;
                dim++;
            }
        }
        while(dim >= 0){
            SpanQuery_SetStack(&it->sq, dim, st);
            dim--;
        }
        it->idx = it->sq.idx;
        it->type.state |= SUCCESS;
        if(it->idx == it->values->max_idx){
            it->type.state |= FLAG_ITER_LAST;
        }
    }
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    Abstract *a = Span_GetFromQ(&it->sq);
    /*
    printf("get: idx:%d/nvalues:%d value:%p\n", it->sq.idx, it->values->nvalues, a);
    */
    return a;
}

status Iter_Reset(Iter *it){
    it->type.state |= END;
    return SUCCESS;
}

Iter *Iter_Init(Iter *it, Span *values){
    memset(it, 0, sizeof(Iter));
    it->type.of = TYPE_ITER;
    it->type.state |= END;
    it->values = asIfc(values, TYPE_SPAN);
    return it;
}

Iter *Iter_InitReverse(Iter *it, Span *values){
    Iter_Init(it, values);
    it->type.state |= FLAG_ITER_REVERSE;
    it->idx = it->values->max_idx+1;
    return it;
}

Iter *Iter_Make(MemCtx *m, Span *values){
    Iter *it = (Iter *)MemCtx_Alloc(m, sizeof(Iter));
    Iter_Init(it, values);
    return it;
}
