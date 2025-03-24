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
        void **ptr = (void **)st->slab;
        ptr += st->localIdx;
        slot = *ptr;
    }
    return StrVec_FmtAdd(m, v, fd, "SpanState<^D._i4^d.dim, ^D._i4^d.localIdx, , ^D._i4^d.offset, ^D._a^d.slot, ^D._a^d.slab>", 
        (i32)st->dim, (i32)st->localIdx, (i32)st->offset, slot, st->slab);
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

    i64 total = 0;
    total += StrVec_FmtAdd(m, v, fd, "Span<_i4values/0.._i4/_i4dims>", 
        p->nvalues, p->max_idx, (i32)p->dims); 
    
    return total;
}


i64 SpanQuery_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    SpanQuery *sq = as(a, TYPE_SPAN_QUERY);
    i64 total = 0;
    total += StrVec_FmtAdd(m, v, fd, "SpanQuery<^D_i4^d: [\n", sq->idx);
    if(sq->span == NULL){
        total += StrVec_FmtAdd(m, v, fd, "span=null");
    }else{
        i8 dim = 0;
        while(dim <= sq->span->dims){
            total += StrVec_FmtAdd(m, v, fd, "  ^D._i4^d.dim: ", (i32)dim);
            SpanState *st = ((SpanState *)sq->stack)+dim;
            if(st != NULL){
                total += SpanState_Print(m, v, fd, 
                    (Abstract *)st, TYPE_SPAN_STATE, extended);
                if(st->slab == sq->span->root){
                    total += StrVec_FmtAdd(m, v, fd, " ^D(root)^d");
                }
            }else{
                total += StrVec_FmtAdd(m, v, fd, "NULL\n");
            }

            if(dim < sq->span->dims){
                total += StrVec_FmtAdd(m, v, fd, ",\n");
            }else{
                total += StrVec_FmtAdd(m, v, fd, "\n");
            }
            dim++;
        }
    }
    total += StrVec_FmtAdd(m, v, fd, "]>");
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

