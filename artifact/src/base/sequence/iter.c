#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    /*
    i8 dim = 0;
    SpanQuery *sq = &it->sq;
    SpanState *st = NULL;
    void **start = NULL;
    void **last = NULL;
    int max_idx = it->values->max_idx;
    if(it->values == NULL || it->values->nvalues == 0){
        goto end;
    }
    if((it->type.state & END) != 0){
        it->idx = 0;
        SpanQuery_Setup(sq, it->values, SPAN_OP_GET, 0);
        Span_Query(sq);
        it->type.state &= ~(END|FLAG_ITER_LAST);

        st = &sq->stack[dim];
        if(*(st->ptr) != 0){
            goto found;
        }
    }
    if((it->idx+1) > it->values->max_idx){
        goto end;
    }else{
        i8 dims = sq->span->dims;
        while(dim <= dims){
            st = &sq->stack[dim];
            i32 increment = _increments[dim];
            start = (void **)st->slab;
            ptr = start+(st->localIdx);
            void **orig = st->ptr;
            last = start+(SPAN_STRIDE-1);

            while(ptr < last){
                if(sq->idx >= max_idx){
                    goto end;
                }
                if(*(++ptr) != NULL){
                    break;
                }
                sq->idx += increment;
            }
            if(*ptr != NULL && ptr != orig){
                i32 localIdx = (i32)(ptr - start);
                sq->idx = st->offset + (localIdx * _increments[dim]);
                break;
            }else{
                dim++;
            }
        }
        while(dim >= 0){
            st = SpanQuery_SetStack(sq, dim);
            dim--;
        }
        it->idx = sq->idx;
        it->type.state |= SUCCESS;
    }
found:
    if(it->idx == it->values->max_idx){
        it->type.state |= FLAG_ITER_LAST;
    }
    return it->type.state;
end:
    it->type.state |= END;
    */
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    return Span_GetFromQ(&it->sq);
}

status Iter_Reset(Iter *it){
    it->type.state |= END;
    return SUCCESS;
}

Iter *Iter_Init(Iter *it, Span *values){
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
