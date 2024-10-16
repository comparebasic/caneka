#include <external.h>
#include <caneka.h>

status Queue_SetDelay(void *sl, quad delayTicks){
    int dim = 0;
    QueueIdx *qidx = (QueueIdx *)sl;
    while(qidx != NULL){
        if(qidx->delayTicks == 0 || delayTicks < qidx->delayTicks){
            qidx->delayTicks = delayTicks;
        }
        qidx++;
    }
    return SUCCESS;
}

status Queue_Update(Queue *q, int idx, byte dim, quad delayTicks){
    if(q->sq.idx != idx && q->sq.dims != dim){
        /* requery */
        SpanQuery_Setup(&(q->sq), (Span *)q, SPAN_OP_SET, idx);
        Span_Query(&(q->sq));
    }

    SpanState *st = SpanQuery_StateByDim(&(q->sq), dim);

    return Queue_SetDelay((QueueIdx *)st->slab, delayTicks);
}

QueueIdx *Queue_Add(Queue *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    SpanQuery *sq = &(q->sq);
    QueueIdx qidx;
    qidx.item = value;
    SpanState *st = sq->nextAvailable;

    if(st->slab == NULL){
        int nextIdx = q->span.max_idx+1;
        return (QueueIdx *)Span_Set((Span *)q, nextIdx, (Abstract *)&qidx); 
    }else{
        SpanQuery_Setup(sq, (Span *)q, SPAN_OP_SET, sq->idx);
        return (QueueIdx *)Span_SetFromQ(sq, (Abstract *)&qidx);
    }
}

status Queue_Remove(Queue *q, int idx){
    SpanQuery *sq = &(q->sq);
    SpanQuery_Setup(sq, (Span *)q, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(sq, NULL);
    return SUCCESS;
}

QueueIdx *Queue_Next(Queue *q){
    if(q->sq.dims != q->span.dims){
        SpanQuery_Refresh(&(q->sq));
    }
    SpanState *st = q->sq.stack;
    if((q->span.type.state & PROCESSING) == 0){
        q->span.type.state |= PROCESSING;
        SpanQuery_Setup(&(q->sq), (Span *)q, SPAN_OP_GET, 0);
        status r = Span_Query(&(q->sq));
        if((r & SUCCESS) == 0){
            q->span.type.state |= ERROR;
            return NULL;
        }
        return Span_GetFromQ(&(q->sq));
    }else{
        byte dim = 0;
outer:
        while(q->sq.idx <= q->span.max_idx){
            int maxIdx = q->span.def->stride;
            if(st->dim != 0){
                maxIdx = q->span.def->idxStride;
            }
            if(st->localIdx+1 < maxIdx){
                st->localIdx++;
                q->sq.idx++;
                if(DEBUG_QUEUE){
                    printf("\x1b[%dmincr dim:%d\n", DEBUG_QUEUE, dim);
                    Debug_Print((void *)(&q->sq), 0, "Underneith: ", DEBUG_QUEUE, TRUE);
                    printf("\n");
                }
                if(dim == 0){
                    if(*((util *)Slab_valueAddr(st->slab, q->span.def, st->localIdx)) != 0){
                        goto final;
                    }
                    goto outer;
                }else{
                    while(TRUE){
                        SpanQuery_SetStack(&(q->sq), dim, 0, 0);
                        if(Slab_nextSlotPtr(st->slab, q->span.def, st->localIdx) != NULL){
                            while(dim >= 1){
                                dim--;
                                SpanQuery_SetStack(&(q->sq), dim, 0, 0);
                            }
                            dim = 0;
                            st = SpanQuery_StateByDim(&(q->sq), dim);
                            if(*((util *)Slab_valueAddr(st->slab, q->span.def, st->localIdx)) != 0){
                                goto final;
                            }
                            goto outer;
                        }
                        q->sq.idx += st->increment;
                    }
                }
                goto final;
            }
            st->localIdx = 0;
            st++;
            dim++;

        }
final:
        if(DEBUG_QUEUE){
            Debug_Print(&(q->sq), 0, "Queue Next SQ: ", DEBUG_QUEUE, TRUE);
            printf("\n");
        }

        if(q->sq.idx > q->span.max_idx){
            q->span.type.state |= END;
            q->span.type.state &= ~PROCESSING;
            return NULL;
        }else{
            return Span_GetFromQ(&(q->sq));
        }
    }
}

Queue *Queue_Make(MemCtx *m, GetDelayFunc getDelay){
    Queue *q = MemCtx_Alloc(m, sizeof(Queue));
    /* copied from span.c */
    q->span.m = m;
    q->span.def = SpanDef_FromCls(TYPE_QUEUE_SPAN);
    q->span.type.of = TYPE_QUEUE_SPAN;
    q->span.root = Span_valueSlab_Make(m, q->span.def);
    q->span.max_idx = -1;
    /* */
    q->getDelay = getDelay;

    return q;
}
