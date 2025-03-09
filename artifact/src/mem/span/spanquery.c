#include <external.h>
#include <caneka.h>

SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim){
    if(dim > MAX_DIMS){
        Fatal("Greater dim than available stack", TYPE_SPAN);
    }
    return sq->stack+dim;
}

SpanState *SpanQuery_SetStack(SpanQuery *sq, byte dim){
    Span *p = sq->span;

    void *sl = NULL;
    word localIdx = 0;
    SpanState *st = SpanQuery_StateByDim(sq, dim);
    int increment = Span_availableByDim(dim, SPAN_STRIDE);

    if(dim == p->dims){
        SpanState *st = SpanQuery_StateByDim(sq, p->dims);
        sl = p->root;
        localIdx = sq->idx / increment;
        st->offset = localIdx * increment;
    }else{
        SpanState *prev = SpanQuery_StateByDim(sq, dim+1);

        localIdx = ((sq->idx - prev->offset) / increment);
        int localMax = SPAN_STRIDE;
        if(localIdx >= localMax){
            printf("LocalIdx:%d prevLocalIdx:%d offset:%d dim:%d\n", localIdx, prev->localIdx, prev->offset, dim);
            Fatal("local_idx greater than stride max", sq->span->type.of);
            return NULL;
        }
        st->offset = prev->offset + increment*localIdx;
        printf("get %d from dim:%d using slab:%p\n", prev->localIdx, dim, prev->slab);
        sl = (void *)Slab_nextSlot(prev->slab, prev->localIdx);
    }

    st->slab = sl;
    st->localIdx = localIdx;
    st->dim = dim;
    st->increment = increment;

    return st;
}

void SpanQuery_Setup(SpanQuery *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SpanQuery));

    sr->type.of = TYPE_SPAN_QUERY;
    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->idx = idx;
    sr->dimsNeeded = Span_GetDimNeeded(idx);

    return;
}

status Span_Query(SpanQuery *sr){
    MemCtx *m = sr->span->m;
    if(sr->dimsNeeded > sr->dims){
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return NOOP;
        }
        Span_GrowToNeeded(sr);
    }
    return Span_Extend(sr);
}
