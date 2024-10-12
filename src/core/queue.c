#include <external.h>
#include <caneka.h>

status Queue_SetDelay(void **stack, quad delayTicks){
    void **sl = stack;
    int dim = 0;
    while(*sl != NULL){
        QueueIdx *qidx = *sl;
        if(qidx->delayTicks == 0 || delayTicks < qidx->delayTicks){
            qidx->delayTicks = delayTicks;
        }
        sl++;
    }
    return SUCCESS;
}

status Queue_Update(Queue *q, int idx, byte dims, quad delayTicks){
    if(q->sr.idx != idx && q->sr.dims != dims){
        /* requery */
        SlabResult_Setup(&(q->sr), (Span *)q, SPAN_OP_SET, idx);
        Span_Query(&(q->sr));
    }
    return Queue_SetDelay(q->sr.stack, delayTicks);
}

static status Queue_FindNext(Queue *q, void *sl, byte dim){
    status r = READY;
    QueueIdx *qidx = (QueueIdx *)sl;
    for(int i = 0; i < q->span.def->idxStride; i++){
        if((qidx->flags & SLAB_FULL) != 0){
            continue;
        }else{
            if(dim == 0){
                if(qidx->item == NULL){
                    q->sr.offset += i;
                    return SUCCESS;
                }
            }else{
                int increment = Span_availableByDim(dim, q->span.def->stride, q->span.def->idxStride);
                r = Queue_FindNext(q, qidx->item, dim-1);
                if((r & SUCCESS) != 0){
                    return r;
                }
                q->sr.offset += increment*i;
            }
        }
        qidx++;
    }

    return MISS;
}

void *Queue_Add(Queue *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    SlabResult *sr = &(q->sr);
    status r = Queue_FindNext(q, q->span.root, q->span.dims);
    sr->idx = sr->offset;
    if(r == MISS){
        sr->idx = q->span.max_idx+1;
    }
    QueueIdx qidx;
    qidx.item = value;
    SlabResult_Setup(&(q->sr), (Span *)q, SPAN_OP_SET, sr->idx);
    void *ptr = _span_Set(sr, sr->idx, (Abstract *)&qidx);

    return ptr;
}

void *Queue_Remove(Queue *q, int idx){
    SlabResult *sr = &(q->sr);
    SlabResult_Setup(sr, (Span *)q, SPAN_OP_REMOVE, idx);
    void *ptr = _span_Set(sr, idx, NULL);
    return ptr;
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
