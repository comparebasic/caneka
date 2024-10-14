#include <external.h>
#include <caneka.h>

SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim){
    if(dim > MAX_DIMS){
        Fatal("Greater dim than available stack", TYPE_SPAN);
    }
    return sq->stack+dim;
}

SpanState *SpanQuery_SetStack(SpanQuery *sq, byte dim, word set, word unset){
    printf(">>>Set Stack dim:%d %d\n", dim, sq->idx);
    SpanDef *def = sq->span->def;
    Span *p = sq->span;

    void *sl = NULL;
    word localIdx = 0;
    SpanState *st = SpanQuery_StateByDim(sq, dim);
    int increment = Span_availableByDim(dim, def->stride, def->idxStride);
    printf("dim %d increment %d\n", dim, increment);

    if(dim == p->dims){
        SpanState *st = SpanQuery_StateByDim(sq, p->dims);
        sl = p->root;
        localIdx = sq->idx / increment;
        st->offset = localIdx * increment;
        printf("   localIdx %d offset %d\n", localIdx, st->offset);
        printf("setting up first slot %p\n", sl);
    }else{
        SpanState *prev = SpanQuery_StateByDim(sq, dim+1);

        SpanState_Print(prev, sq->span->def, COLOR_YELLOW);
        printf("\n");

        localIdx = ((sq->idx - prev->offset) / increment);
        printf("   localIdx %d\n", localIdx);
        if(localIdx >= sq->span->def->idxStride){
            printf("LocalIdx %d offset %d dim: %d\n", localIdx, prev->offset, dim);
            Fatal("local_idx greater than idxStride", sq->span->type.of);
        }
        st->offset = prev->offset + increment*localIdx;
        sl = (void *)Slab_nextSlot(prev->slab, sq->span->def, prev->localIdx);
        printf("setting up second slot %p localIdx:%d\n", sl, localIdx);
    }

    st->slab = sl;
    st->localIdx = localIdx;
    printf("localIdx %d vs %d\n", localIdx, st->localIdx);
    st->flags |= set;
    st->flags &= ~unset;

    printf("slab is %p\n", st->slab);
    SpanState_Print(st, sq->span->def, COLOR_BLUE);
    printf("\n");
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

