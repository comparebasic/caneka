#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    if(it->values == NULL || it->values->nvalues == 0){
        it->type.state |= END;
        return it->type.state;
    }
    if((it->type.state & END) != 0){
        if((it->type.state & FLAG_ITER_REVERSE) != 0){
            it->idx = it->values->max_idx;
        }else{
            it->idx = 0;
        }
        SpanQuery_Setup(&it->sq, it->values, SPAN_OP_GET, 0);
        Span_Query(&it->sq);
        it->type.state &= ~(END|FLAG_ITER_LAST);
    }
    if((it->type.state & FLAG_ITER_REVERSE) != 0){
        if(it->idx == 0){
            it->type.state |= END;
            return it->type.state;
        }

        byte dim = 0;
        SpanQuery *sq = &it->sq;
        SpanState *st = NULL;
        while(dim <= sq->dims){
            st = sq->stack[dim];
            slot *start = (slot *)st->slab;
            slot *ptr = start+st->localIdx;
            while(ptr > start && 
                st->localIdx-- && sq->idx -= st->increment && *(--ptr) == NULL){};
            if(ptr != NULL && ptr != start){
                break;
            }else{
                sq->idx += (SPAN_STRIDE-1)-st->localIdx;
                st->localIdx = SPAN_STRIDE-1;
                dim++;
            }
        }
        while(i >= 0){
            SpanQuery_SetStack(&it->sq, i, st);
            i--;
        }
        it->idx = it->sq.idx;
        it->type.state |= SUCCESS;

        if(it->idx == 0){
            it->type.state |= FLAG_ITER_LAST;
        }
    }else{
        if((it->idx+1) > it->values->max_idx){
            it->type.state |= END;
        }else{
            byte dim = 0;
            SpanQuery *sq = &it->sq;
            SpanState *st = NULL;
            while(dim <= sq->dims){
                st = sq->stack[dim];
                slot *start = (slot *)st->slab;
                slot *ptr = start+st->localIdx;
                slot *last = start+(SPAN_STRIDE-1);
                start = ptr;
                while(ptr < last && 
                    st->localIdx++ && sq->idx += st->increment && *(++ptr) == NULL){};
                if(ptr != NULL && ptr != start){
                    break;
                }else{
                    sq->idx -= st->localIdx;
                    st->localIdx = 0;
                    dim++;
                }
            }
            while(i >= 0){
                SpanQuery_SetStack(&it->sq, i, st);
                i--;
            }
            it->idx = it->sq.idx;
            it->type.state |= SUCCESS;
            if(it->idx == it->values->max_idx){
                it->type.state |= FLAG_ITER_LAST;
            }

        }
    }
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    Abstract *a = Span_GetFromQ(&it->sq);
    printf("get: %d/%d %p\n", it->idx, it->values->nvalues, a);
    return a;
}

status Iter_Reset(Iter *it){
    it->idx = -1;
    it->type.state = 0;
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
