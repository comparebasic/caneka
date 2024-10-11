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
        SlabResult_Setup(&(q->sr), p, SPAN_OP_SET, idx);
        Span_Query(&(q->sr));
    }
    return Queue_SetDelay(q->sr.stack, delayTicks);
}

int Queue_FindNext(Span *p){
    /* climb through each dim and set the nextAvailable or active/full flags
    then return the open index
    */
    return 0;
}

void *Queue_Add(Queue *q, void *value, quad delayTicks){
    if(value == NULL){
        return NULL;
    }

    SlabResult *sr = &(q->sr);
    SlabResult_Setup(q->sr, p, SPAN_OP_SET, idx);

    int idx = Queue_FindNext(q);
    void *ptr = _span_Set(sr, idx, (Abstract *)value);

    return ptr;
}

void *Queue_Make(MemCtx *m, GetDelayFunc getDelay){
    Queue *q = MemCtx_Alloc(m, sizeof(Queue));
    /* copied from span.c */
    q->m = m;
    q->def = SpanDef_FromCls(TYPE_QUEUE_SPAN);
    q->type.of = p->def->typeOf;
    q->root = Span_valueSlab_Make(m, p->def);
    q->max_idx = -1;
    /* */
    q->getDelay = getDelay;

    return q;
}
