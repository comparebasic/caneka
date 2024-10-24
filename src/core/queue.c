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
        QueueIdx *ptr = (QueueIdx *)Span_Set(q->span, nextIdx, (Abstract *)&qidx); 
        if(ptr != NULL){
            q->count++;
        }
        return ptr;
    }else{
        q->available.op = SPAN_OP_SET;
        if(DEBUG_QUEUE){
            printf("\x1b[%dmAdding to open slot %d\x1b[%dm\n", DEBUG_QUEUE, q->current.idx, DEBUG_QUEUE);
        }
        QueueIdx *ptr = (QueueIdx *)Span_SetFromQ(&q->available, (Abstract *)&qidx);
        if(ptr != NULL){
            memset(&q->available, 0, sizeof(SpanQuery));
            q->count++;
        }
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
    q->count--;
    return SUCCESS;
}

QueueIdx *Queue_Next(Queue *q, SkipSlabFunc skip){
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
        if(skip != NULL && skip(q->source, 0) > 0){
            q->current.idx += Span_availableByDim(1, q->span->def->stride, q->span->def->idxStride);
            return Queue_Next(q, skip); 
        }
        return Span_GetFromQ(&q->current);
    }else{
        /* Use the Span_GetFromQ function to find the next value, jumping by increment amounts if it finds missing slabs */
        int maxIdx = Span_Capacity(q->span);
        while(TRUE){
            q->current.idx += Span_availableByDim(q->current.queryDim, q->span->def->stride, q->span->def->idxStride);
            if(q->current.idx >= maxIdx){
                goto end;
            }
            while(TRUE){
                SpanQuery_Setup(&q->current, q->span, SPAN_OP_GET, q->current.idx);
                if((Span_Query(&q->current) & MISS) == 0){
                    break;
                }
                /* capture an available slot if we just found one in dim 0*/
                if(q->current.queryDim == 0 && q->available.stack[0].slab == NULL){
                    SpanQuery_Setup(&q->available, q->span, SPAN_OP_GET, q->current.idx);
                    Span_Query(&q->available);
                }
                int increment = Span_availableByDim(q->current.queryDim, q->span->def->stride, q->span->def->idxStride);
                q->current.idx += increment;
                /*
                while(skip != NULL && q->current.idx+increment < maxIdx && skip(q->source, q->current.idx) <= 0){
                    q->current.idx += increment;
                }
                */
                if(q->current.idx >= maxIdx){
                    goto end;
                }
            }
            QueueIdx *qidx = (QueueIdx *)Span_GetFromQ(&q->current);
            if(qidx != NULL && qidx->item != NULL){
               return qidx; 
            }
        }
end:
        q->type.state |= END;
        q->type.state &= ~PROCESSING;
        return NULL;
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
