#include <external.h>
#include <caneka.h>

i64 Iter_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    return StrVec_FmtAdd(m, v, fd, "I<_t:_i4 of _i4>",
        State_ToStr(m, it->type.state), it->idx, it->values->nvalues);
}

i64 SpanState_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    SpanState *st = (SpanState *)a;
    i32 idx = st->localIdx;
    void *slot = NULL;
    if(st->slab != NULL){
        slot = *(((void **)st->slab) + st->localIdx);
    }
    return StrVec_FmtAdd(m, v, fd, "SpanState<^B_i4^b^o+idx, ^B_a^b^slot, ^B_a^b^slab, ^B_i4^b^localIdx, ^B_i4^b^offset>", 
        idx, slot, st->slab, (i32)st->localIdx, (i32)st->offset);
}

i64 Slab_Print(MemCtx *m, StrVec *v, i32 fd, slab *slab, i8 dim, i8 dims){
    /*
    printf("\x1b[%dm(%p){", color, slab);
    void **slot = (void **)slab;
    void **end = slot+SPAN_STRIDE;
    i32 i = 0;

    while(slot < end){
        if(*slot != NULL){
            if(slot != (void **)slab){
                printf(", ");
            }
            printf("\x1b[1;%dm%d\x1b[0;%dm=%p", color, i, color, *slot);
        }
        i++;
        slot++;
    }
    printf("}\x1b[0m");

    if(dim > 0){
        slot = (void **)slab;
        end = slot+SPAN_STRIDE;
        i = 0;
        while(slot < end){
            if(*slot != NULL){
                printf("\n");
                wsOut(m, v, dims-(dim-1));
                printf("\x1b[%dm{\n", color);
                wsOut(m, v, dims-(dim-1)+1);
                printf("%d=", i);
                Slab_Print(m, v, *slot, dim-1, dims, color); 
            }
            i++;
            slot++;
        }
        printf("\n");
    }

    printf("\x1b[%dm}\x1b[0m", color);
    */
    return 0;
}

i64 Span_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Span *p = (Span*)as(a, TYPE_SPAN); 
    /*
    printf("\x1b[%dm%sSpan<\x1b[1;%dm%d\x1b[0;%dmval/\x1b[1;%dm%hd\x1b[0;%dmdim ", color, msg, color, p->nvalues, color, color, p->dims, color);
    Slab_Print(m, v, p->root, p->dims, p->dims, color);
    printf("\x1b[%dm>\x1b[0m", color);
    */
    return 0;
}


i64 SpanQuery_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    SpanQuery *sq = as(a, TYPE_SPAN_QUERY);
    /*
    printf("\x1b[%dm%sSQ<%s idx:%d op:%d dims:%hu/%hu", color, msg, State_ToChars(sq->type.state), sq->idx, sq->op, sq->dims, sq->dimsNeeded);
    SpanState *st = sq->stack;
    for(int i = 0; i <= sq->span->dims; i++){
        printf("\n");
        indent_Print(1);
        printf("\x1b[%dm%d: ", color, i);
        spanState_Print(st, color);
        st++;
    }
    printf("\n");
    printf("\x1b[%dm>\x1b[0m", color);
    */
    return 0;
}

i64 Lookup_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    /*
    printf("\x1b[%dm%sLk<offset:%d latest_idx:\%d", color, msg, lk->offset, lk->latest_idx);
    Debug_Print((void *)lk->values, 0, "", color, TRUE);
    printf("\x1b[%dm>\x1b[0m", color);
    */
    return 0;
}


status SequenceDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_QUERY, (void *)SpanQuery_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_STATE, (void *)SpanState_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);

    return r;
}

