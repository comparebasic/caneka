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

status Queue_Update(SpanQuery *q, int idx, byte dim, quad delayTicks){
    if(q.idx != idx && q->dims != dim){
        /* requery */
        SpanQuery_Setup(&(q), (Span *)q, SPAN_OP_SET, idx);
        Span_Query(&(q));
    }

    SpanState *st = SpanQuery_StateByDim(q, dim);

    return Queue_SetDelay((QueueIdx *)st->slab, delayTicks);
}

QueueIdx *Queue_Add(SpanQuery *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    QueueIdx qidx;
    qidx.item = value;
    SpanState *st = q->nextAvailable;

    if(st->slab == NULL){
        int nextIdx = q->span.max_idx+1;
        return (QueueIdx *)Span_Set((Span *)q, nextIdx, (Abstract *)&qidx); 
    }else{
        SpanQuery_Setup(q, (Span *)q, SPAN_OP_SET, q->idx);
        return (QueueIdx *)Span_SetFromQ(q, (Abstract *)&qidx);
    }
}

status Queue_Remove(SpanQuery *q, int idx){
    SpanQuery_Setup(q, (Span *)q, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(q, NULL);
    return SUCCESS;
}

QueueIdx *Queue_Next(SpanQuery *q){
    if(q->dims != q->dims){
        SpanQuery_Refresh(q);
    }
    SpanState *st = q->stack;
    if((q->span.type.state & PROCESSING) == 0){
        q->span.type.state |= PROCESSING;
        SpanQuery_Setup(q, q->span, SPAN_OP_GET, 0);
        status r = Span_Query(q);
        if((r & SUCCESS) == 0){
            q->span.type.state |= ERROR;
            return NULL;
        }
        return Span_GetFromQ(q);
    }else{
        byte dim = 0;
outer:
        while(q->idx <= q->span->max_idx){
            int maxIdx = q->span->def->stride;
            if(st->dim != 0){
                maxIdx = q->span->def->idxStride;
            }
            if(st->localIdx+1 < maxIdx){
                st->localIdx++;
                q->idx++;
                if(DEBUG_QUEUE){
                    printf("\x1b[%dmincr dim:%d\n", DEBUG_QUEUE, dim);
                    Debug_Print((void *)q, 0, "Underneith: ", DEBUG_QUEUE, TRUE);
                    printf("\n");
                }
                if(dim == 0){
                    if(*((util *)Slab_valueAddr(st->slab, q->span.def, st->localIdx)) != 0){
                        goto final;
                    }
                    goto outer;
                }else{
                    while(TRUE){
                        SpanQuery_SetStack(q, dim, 0, 0);
                        if(Slab_nextSlotPtr(st->slab, q->span.def, st->localIdx) != NULL){
                            while(dim >= 1){
                                dim--;
                                SpanQuery_SetStack(q, dim, 0, 0);
                            }
                            dim = 0;
                            st = SpanQuery_StateByDim(q, dim);
                            if(*((util *)Slab_valueAddr(st->slab, q->span.def, st->localIdx)) != 0){
                                goto final;
                            }
                            goto outer;
                        }
                        q->idx += st->increment;
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
            Debug_Print(q, 0, "Queue Next SQ: ", DEBUG_QUEUE, TRUE);
            printf("\n");
        }

        if(q->idx > q->span->max_idx){
            q->span.type.state |= END;
            q->span.type.state &= ~PROCESSING;
            return NULL;
        }else{
            return Span_GetFromQ(q);
        }
    }
}
