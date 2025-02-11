#include <external.h>
#include <caneka.h>

void TableChain_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    TableChain *tch = (TableChain *)as(a, TYPE_TABLE_CHAIN);
    printf("\x1b[%dmTChain<", color);
    if(extended){
        while(tch != NULL){
            printf("\n    ");
            Debug_Print((Abstract *)tch->tbl, 0, "tbl: ", color, extended);
            tch = tch->next;
        }
    }
    if(extended){
        while(tch != NULL){
            printf("\x1b[%dmtbl(%d)", color, tch->tbl != NULL ? tch->tbl->nvalues : 0);
            if(tch != NULL){
                printf("\x1b[%dm, ", color);
            }
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);

}

void spanState_Print(SpanState *st, int color){
    printf("\x1b[%dmST<%p localIdx:%hu increment:%d offset:%hu dim:%d>\x1b[0m", color, st->slab, st->localIdx, st->increment, st->offset, st->dim);
}

void SpanState_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SpanState *st = (SpanState *)a;
    printf("\x1b[%dm%s", color, msg);
    spanState_Print(st, color);
}

void SpanQuery_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SpanQuery *sq = as(a, TYPE_SPAN_QUERY);
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
}

static void Slab_Print(void *sl, SpanDef *def, int color, byte dim, int parentIdx, byte indent, boolean extended, byte totalDims, int offset){
    indent_Print(indent);
    printf("%d(%d)= ", parentIdx,offset);
    if(dim == 0){
        if(extended){
            printf("#%p->", sl);
        }
        printf("Values[ ");
        void *ptr = sl;
        boolean any = FALSE;
        for(int i = 0; i < def->stride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                printf("%d=", i);
                Abstract *t = (Abstract *)ptr;
                if(def->typeOf == TYPE_QUEUE_SPAN){
                    QueueIdx *qidx = (QueueIdx *)t;
                    Debug_Print((void *)qidx->item, 0, "", color, FALSE);
                    printf("\x1b[%dm", color);
                }else{
                    if((def->flags & SPAN_INLINE) == 0){
                        t = *((Abstract **)t);
                    }
                    i64 n = (util)t;
                    if(def->itemSize == sizeof(int)){
                        printf("%u", (int)n);
                    }else if((def->flags & SPAN_RAW) == 0 && t->type.of != 0){
                        Debug_Print((void *)t, 0, "", color, FALSE);
                    }else{
                        if(def->slotSize > 1){
                            printf("\x1b[%dm%ldbytes", color, def->slotSize*sizeof(void *));
                        }else{
                            Bits_Print((byte *)a, sizeof(void *)*def->slotSize, "", color, FALSE);
                        }
                    }
                }
                printf(" ");
            }
            ptr += sizeof(void *)*def->slotSize;
        }
        printf("]");
    }else{
        if(extended){
            printf("#%p->", sl);
        }
        printf("Idx(%d/%d)[ ",  dim, Span_availableByDim(dim, def->stride, def->idxStride));
        boolean any = FALSE;
        void *ptr = sl;
        for(int i = 0; i < def->idxStride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                int increment = Span_availableByDim(dim, def->stride, def->idxStride);
                util pos = offset+increment*i;
                if(def->typeOf == TYPE_QUEUE_SPAN){
                    QueueIdx *qidx = (QueueIdx *)a;
                    printf("%d=%ld..%ld!%s/%d", i, pos, pos+(increment-1), QueueFlags_ToChars(qidx->flags), qidx->delayTicks);
                }else{
                    printf("%d=%ld..%ld", i, pos, pos+(increment-1));
                }
                if(i < def->idxStride - 1){
                    printf(" ");
                }
            }
            ptr += sizeof(void *)*def->idxSize;
        }
        ptr = sl;
        any = FALSE;
        for(int i = 0; i < def->idxStride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                printf("\n");
                offset += Span_availableByDim(dim, def->stride, def->idxStride)*i;

                Slab_Print((void *)*a, def, color, dim-1, i, indent+1, extended, totalDims, offset);
                any = TRUE;
            }
            ptr += sizeof(void *)*def->idxSize;
        }
        if(any){
            printf("\n");
        }
        indent_Print(indent);
        printf("]");
        if(any){
            printf("\n");
        }
    }
}

void SpanDef_Print(SpanDef *def){
    char *flags = "";
    if((def->flags & SPAN_INLINE) != 0){
        flags = "(inline)";
    }
    printf("def=[idxStride:%d stride:%d idxSize:%d slotSize:%d%s itemSize:%d, valueHdr:%d", 
         def->idxStride, def->stride, def->idxSize, def->slotSize, flags, def->itemSize, def->valueHdr);
    if(def->dim_lookups[0] != 0){
        printf(" lookups: ");
        for(int i = 0; i < 8; i ++){
            printf("%lu ", def->dim_lookups[i]);
        }
    }
    printf("]");
}

void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    printf("\x1b[0;%dm%sP<%u items in dims:%u ", color, msg,
        p->nvalues, p->dims);
    if(extended){
        if(p->metrics.selected >= 0){
            printf("sel:%d ", p->metrics.selected);
        }
        if(p->def->typeOf == TYPE_QUEUE_SPAN){
            printf("%s ", QueueFlags_ToChars(p->flags));
        }
        SpanDef_Print(p->def);
        printf("\n");
        Slab_Print(p->root, p->def, color, p->dims, 0, 1, TRUE, p->dims, 0);
    }
    printf(">\x1b[0m");
}

void Lookup_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    printf("\x1b[%dm%sLk<offset:%d latest_idx:\%d", color, msg, lk->offset, lk->latest_idx);
    Debug_Print((void *)lk->values, 0, "", color, TRUE);
    printf("\x1b[%dm>\x1b[0m", color);
}

