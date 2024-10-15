#include <external.h>
#include <caneka.h>

SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim){
    if(dim > MAX_DIMS){
        Fatal("Greater dim than available stack", TYPE_SPAN);
    }
    return sq->stack+dim;
}

status SpanQuery_Refresh(SpanQuery *sq){
    return SUCCESS;
}

SpanState *SpanQuery_SetStack(SpanQuery *sq, byte dim, word set, word unset){
    SpanDef *def = sq->span->def;
    Span *p = sq->span;

    void *sl = NULL;
    word localIdx = 0;
    SpanState *st = SpanQuery_StateByDim(sq, dim);
    int increment = Span_availableByDim(dim, def->stride, def->idxStride);

    if(dim == p->dims){
        SpanState *st = SpanQuery_StateByDim(sq, p->dims);
        sl = p->root;
        localIdx = sq->idx / increment;
        st->offset = localIdx * increment;
    }else{
        SpanState *prev = SpanQuery_StateByDim(sq, dim+1);

        localIdx = ((sq->idx - prev->offset) / increment);
        int localMax = sq->span->def->stride;
        if(dim > 0){
            localMax = sq->span->def->idxStride;
        }
        if(localIdx >= localMax){
            printf("LocalIdx:%d prevLocalIdx:%d offset:%d dim:%d\n", localIdx, prev->localIdx, prev->offset, dim);
            Fatal("local_idx greater than stride max", sq->span->type.of);
        }
        st->offset = prev->offset + increment*localIdx;
        sl = (void *)Slab_nextSlot(prev->slab, sq->span->def, prev->localIdx);
    }

    st->slab = sl;
    st->localIdx = localIdx;
    st->flags |= set;
    st->flags &= ~unset;
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
    sr->dimsNeeded = SpanDef_GetDimNeeded(p->def, (idx));

    return;
}

status Span_Query(SpanQuery *sr){
    MemCtx *m = sr->span->m;
    if(sr->dimsNeeded > sr->dims){
        if(sr->op == SPAN_OP_GET){
            return MISS;
        }
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return MISS;
        }
        Span_GrowToNeeded(sr);
    }
    return Span_Extend(sr);
}

