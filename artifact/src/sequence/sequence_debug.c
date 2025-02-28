#include <external.h>
#include <caneka.h>

char *QueueFlags_ToChars(word flags){
    String *s = String_Init(DebugM, 64);
    if((flags & SLAB_ACTIVE) != 0){
        String_AddBytes(DebugM, s, bytes("A"), 1);
    }
    if((flags & SLAB_FULL) != 0){
        String_AddBytes(DebugM, s, bytes("F"), 1);
    }

    if(s->length == 0){
        return "ZERO";
    }

    return (char *)s->bytes;
}

static void Iter_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    printf("\x1b[%dm%sI<%s:%d of %d>\x1b[0m", color, msg,
        State_ToChars(it->type.state), it->idx, it->values->nvalues);
}

static void Queue_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Queue *q = as(a, TYPE_QUEUE);
    printf("\x1b[%dm%sQ<%s\x1b[0m", color, msg, State_ToChars(q->type.state));
    if(extended){
        Debug_Print((void *)q->span, 0, "", color, TRUE);
        printf("\n");
        Debug_Print((void *)&q->current, 0, "current=", color, FALSE);
        printf("\n");
        Debug_Print((void *)&q->available, 0, "available=", color, FALSE);
        printf("\n");
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm values:%d current:%d available:%d>\x1b[0m", color, q->span->nvalues, q->current.idx, q->available.idx);
    }
}



void NestedD_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    NestedD *nd = (NestedD *)as(a, TYPE_NESTEDD);
    printf("\x1b[%dmND<%s: ", color, State_ToChars(nd->type.state));
    Debug_Print((void *)nd->current_tbl, 0, "", color, extended);
    Iter it;
    Iter_InitReverse(&it, nd->stack);
    printf("\x1b[%dm stack/%d=(", color, nd->stack->nvalues);
    while((Iter_Next(&it) & END) == 0){
        NestedState *ns = (NestedState *)Iter_Get(&it);
        printf("\x1b[%dm%s -> ", color, NestedD_opToChars(ns->flags));
        Debug_Print((void *) ns->t, 0, "", color, FALSE);
        if((it.type.state & FLAG_ITER_LAST) == 0){
            printf("\x1b[%dm,", color);
        }
    }
    printf("\x1b[%dm)>\x1b[0m", color);
}



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

void OrdTable_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    OrdTable *otbl = (OrdTable *)as(a, TYPE_ORDERED_TABLE);
    Debug_Print((void *)otbl->order, 0, msg, color,extended); 
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

static void mess_PrintRecurse(Mess *ms, char *msg, int color, boolean extended, int indent){
    indent_Print(indent);
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    Debug_Print((void *)ms->name, 0, "", color, extended);
    printf(" ");
    if(ms->atts != NULL){
        printf("\x1b[%dmatts=[", color);
    }
    Iter it;
    Iter_Init(&it, ms->atts);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Debug_Print((void *)h, 0, "", color, extended);
        }
    }
    printf("\x1b[%dm, ", color);
    if(ms->atts != NULL){
        printf("\x1b[%dm]", color);
    }
    Debug_Print(ms->body, 0, "body=", color, TRUE);
    Mess *child = ms->firstChild;
    if(child != NULL){
        printf("\x1b[%dm [", color);
        printf("\n");
    }
    while(child != NULL){
        mess_PrintRecurse(child, "", color, extended, indent+1);
        child = child->next;
    }
    if(ms->next != NULL){
        printf("\x1b[%dmnext=%p", color, ms->next);
    }
    if(ms->firstChild != NULL){
        indent_Print(indent);
        printf("\x1b[%dm]", color);
        printf(">\x1b[0m");
    }
    printf("\n");
}

static void Mess_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Mess *ms = (Mess *)a;
    mess_PrintRecurse(ms, msg, color, extended, 0);
}



status SequenceDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_QUERY, (void *)SpanQuery_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_STATE, (void *)SpanState_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    r |= Lookup_Add(m, lk, TYPE_MINI_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE_CHAIN, (void *)TableChain_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_NESTEDD, (void *)NestedD_Print);
    r |= Lookup_Add(m, lk, TYPE_ORDERED_TABLE, (void *) OrdTable_Print);

    return r;
}

