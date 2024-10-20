#include <external.h>
#include <caneka.h>

status Queue_SetDelay(SpanQuery *sq, quad delayTicks){
    return SUCCESS;
}

status Queue_Update(Queue *q, int idx, byte dim, quad delayTicks){
    if(q->current.idx != idx || q->span->dims != dim){
        SpanQuery_Setup(&q->current, q->span, SPAN_OP_SET, idx);
        Span_Query(&q->current);
    }

    return Queue_SetDelay(&q->current, delayTicks);
}

QueueIdx *Queue_Add(Queue *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    QueueIdx qidx;
    qidx.item = value;
    SpanState *st = (SpanState *)&q->available.stack;

    if(st->slab == NULL){
        int nextIdx = q->span->max_idx+1;
        if(DEBUG_QUEUE){
            printf("\x1b[%dmAdding to %d\x1b[%dm\n", DEBUG_QUEUE, nextIdx, DEBUG_QUEUE);
        }
        return (QueueIdx *)Span_Set(q->span, nextIdx, (Abstract *)&qidx); 
    }else{
        q->available.op = SPAN_OP_SET;
        if(DEBUG_QUEUE){
            printf("\x1b[%dmAdding to open slot %d\x1b[%dm\n", DEBUG_QUEUE, q->current.idx, DEBUG_QUEUE);
        }
        QueueIdx *ptr = (QueueIdx *)Span_SetFromQ(&q->available, (Abstract *)&qidx);
        memset(&q->available, 0, sizeof(SpanQuery));
        return ptr;
    }
}

status Queue_Remove(Queue *q, int idx){
    if(DEBUG_QUEUE){
        printf("Removing idx %d\n", idx);
        Debug_Print((void *)q, 0, "Queue: %s\n", DEBUG_QUEUE, TRUE);
        printf("\n");
    }
    SpanQuery_Setup(&q->available, q->span, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(&q->available, NULL);
    if(DEBUG_QUEUE){
        printf("Removed idx %d\n", idx);
        Debug_Print((void *)&q->available, 0, "Freshly removed: ", COLOR_PURPLE, TRUE);
        printf("\n");
    }
    return SUCCESS;
}

QueueIdx *Queue_Next(Queue *q){
    if(DEBUG_QUEUE){
        Debug_Print((void *)q, 0, "Queue_Next called:", DEBUG_QUEUE, FALSE);
    }
    if(q->current.dims != q->span->dims){
        SpanQuery_Setup(&q->current, q->span, SPAN_OP_REMOVE, q->current.idx);
    }

    SpanState *st = (SpanState *)&q->current.stack;
    if((q->type.state & PROCESSING) == 0){
        SpanQuery_Setup(&q->current, q->span, SPAN_OP_GET, 0);
        q->type.state |= PROCESSING;
        status r = Span_Query(&q->current);
        if((r & SUCCESS) == 0){
            q->type.state |= ERROR;
            return NULL;
        }
        return Span_GetFromQ(&q->current);
    }else{
        byte dim = 0;
outer:
        while(q->current.idx <= q->span->max_idx){
            int maxIdx = q->span->def->stride;
            if(st->dim != 0){
                maxIdx = q->span->def->idxStride;
            }
            if(st->localIdx+1 < maxIdx){
                st->localIdx++;
                q->current.idx++;
                if(DEBUG_QUEUE){
                    printf("\x1b[%dmincr dim:%d\n", DEBUG_QUEUE, dim);
                    Debug_Print((void *)q, 0, "Underneith: ", DEBUG_QUEUE, TRUE);
                    printf("\n");
                }
                if(dim == 0){
                    if(*((util *)Slab_valueAddr(st->slab, q->span->def, st->localIdx)) != 0){
                        goto final;
                    }
                    goto outer;
                }else{
                    while(TRUE){
                        SpanQuery_SetStack(&q->current, dim, 0, 0);
                        if(Slab_nextSlotPtr(st->slab, q->span->def, st->localIdx) != NULL){
                            while(dim >= 1){
                                dim--;
                                SpanQuery_SetStack(&q->current, dim, 0, 0);
                            }
                            dim = 0;
                            st = SpanQuery_StateByDim(&q->current, dim);
                            if(*((util *)Slab_valueAddr(st->slab, q->span->def, st->localIdx)) != 0){
                                goto final;
                            }
                            goto outer;
                        }
                        q->current.idx += st->increment;
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

        if(q->current.idx > q->span->max_idx){
            if(DEBUG_QUEUE){
                Debug_Print((void *)q, 0, "END: ", DEBUG_QUEUE, TRUE);
            }
            q->type.state |= END;
            q->type.state &= ~PROCESSING;
            return NULL;
        }else{
            return Span_GetFromQ(&q->current);
        }
    }
}

status Queue_Init(MemCtx *m, Queue *q, GetDelayFunc getDelay){
    memset(q, 0, sizeof(Queue));
    q->type.of = TYPE_QUEUE;
    q->span = Span_Make(m, TYPE_QUEUE_SPAN);;
    SpanQuery_Setup(&q->current, q->span, SPAN_OP_SET, 0); 
    memset(&q->available, 0, sizeof(SpanQuery)); 
    q->getDelay = getDelay;
    return SUCCESS;
}
