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

static int queue_findSlab(Queue *q, void *sl, byte dim){
    QueueIdx *qidx = (QueueIdx *)sl;
    for(int i = 0; i < q->span.def->idxStride; i++){
        if((qidx->flags & SLAB_FULL) != 0){
            continue;
        }else{
            if(dim == 0){
                if(qidx->item == NULL){
                    q->sr.offset += i;
                    return q->sr.offset;
                }
            }else{
                int increment = Span_availableByDim(dim, q->span.def->stride, q->span.def->idxStride);
                q->sr.offset += increment*i;
                return queue_findSlab(q, qidx->item, dim-1);
            }
        }
        qidx++;
    }

    return q->span.max_idx+1;
}

int Queue_FindNext(Queue *q){
    /* climb through each dim and set the nextAvailable or active/full flags
    then return the open index
    */
    SlabResult *sr = &(q->sr);

    int idx = queue_findSlab(q, q->span.root, q->span.dims);
    SlabResult_Setup(&(q->sr), (Span *)q, SPAN_OP_SET, idx);

    return idx;
}

void *Queue_Add(Queue *q, Abstract *value){
    if(value == NULL){
        return NULL;
    }

    int idx = Queue_FindNext(q);
    if(idx == -1){
        return NULL;
    }
    SlabResult *sr = &(q->sr);
    QueueIdx qidx;
    qidx.item = value;
    void *ptr = _span_Set(sr, idx, (Abstract *)&qidx);

    return ptr;
}

void *Queue_Remove(Queue *q, int idx){
    SlabResult *sr = &(q->sr);
    SlabResult_Setup(sr, (Span *)q, SPAN_OP_SET, idx);
    status r = Span_Query(sr);
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
