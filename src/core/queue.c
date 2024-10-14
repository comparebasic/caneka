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
    if(q->sr.idx != idx && q->sr.dims != dim){
        /* requery */
        SpanQuery_Setup(&(q->sr), (Span *)q, SPAN_OP_SET, idx);
        Span_Query(&(q->sr));
    }

    SpanState *st = SpanQuery_StateByDim(&(q->sr), dim);

    return Queue_SetDelay((QueueIdx *)st->slab, delayTicks);
}

void *Queue_Add(Queue *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    SpanQuery *sr = &(q->sr);
    QueueIdx qidx;
    qidx.item = value;
    SpanState *st = sr->nextAvailable;

    if(st->slab == NULL){
        int nextIdx = q->span.max_idx+1;
        return Span_Set((Span *)q, nextIdx, (Abstract *)&qidx); 
    }else{
        SpanQuery_Setup(sr, (Span *)q, SPAN_OP_SET, sr->idx);
        return Span_SetFromQ(sr, (Abstract *)&qidx);
    }
}

void *Queue_Remove(Queue *q, int idx){
    SpanQuery *sr = &(q->sr);
    SpanQuery_Setup(sr, (Span *)q, SPAN_OP_REMOVE, idx);
    void *ptr = Span_SetFromQ(sr, NULL);
    return ptr;
}

void *Queue_Next(Queue *q){
    if(q->sr.dims != q->span.dims){
        SpanQuery_Refresh(&(q->sr));
    }
    SpanState *st = q->sr.stack;
    if((q->span.type.state & PROCESSING) == 0){
        q->span.type.state |= PROCESSING;
        SpanQuery_Setup(&(q->sr), (Span *)q, SPAN_OP_GET, 0);
        return Span_GetFromQ(&(q->sr));
    }else{
        byte dim = 0;
        byte found = FALSE; 
        while(!found && q->sr.idx <= q->span.max_idx){
            int maxIdx = q->span.def->stride;
            if(st->dim != 0){
                maxIdx = q->span.def->idxStride;
            }
            if(st->localIdx+1 < maxIdx){
                st->localIdx++;
                q->sr.idx += st->increment;
                if(dim != 0){
                    while(st->localIdx < maxIdx){
                        if(Slab_nextSlotPtr(st->slab, q->span.def, st->localIdx) != NULL){
                            found = TRUE;
                        }
                        st->localIdx++;
                    }
                }
            }

            st->localIdx = 0;
            st++;
            dim++;
        }

        if(q->sr.idx > q->span.max_idx){
            q->span.type.state &= END & ~PROCESSING;
            return NULL;
        }else{
            return Span_GetFromQ(&(q->sr));
        }
    }
}

void *Queue_Make(MemCtx *m, GetDelayFunc getDelay){
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
