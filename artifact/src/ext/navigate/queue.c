#include <external.h>
#include <caneka.h>

static gobits Queue_CheckSlab(Queue *q);

status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, void *value){
    status r = READY;
    i32 slabIdx = idx / SPAN_STRIDE;
    MemCh *m = Queue_GetMem(q);
    void *slab = NULL;
    boolean half = (q->type.state & QUEUE_CRIT_HALF) != 0; 
    if((slab = Span_Get(q->crit.data, slabIdx)) == NULL){
        size_t sz = (half ? sizeof(word) : sizeof(util));
        slab = (void *)Bytes_Alloc(m, sizeof(sz*SPAN_STRIDE), TYPE_BYTES_POINTER); 
        r |= Span_Set(q->crit.data, slabIdx, (Abstract *)slab);
    } 
    i32 localIdx = idx-slabIdx*SPAN_STRIDE;
    if(half){
        word *hslab = (word *)slab;
        hslab[localIdx] = *(word *)value;
        r |= SUCCESS;
    }else{
        util *uslab = (util *)slab;
        uslab[localIdx] = *(util *)value;
        r |= SUCCESS;
    }
    return r;
}

status Queue_Next(Queue *q){
    Span *p = q->itemsIt.p;
    q->type.state &= ~(SUCCESS|NOOP);
    if(q->localIdx == SPAN_STRIDE-1){
        q->localIdx = 0;
        q->go = 0;
    }
    if(q->go == 0){
        q->localIdx = 0;
        if(q->itemsIt.type.state & END){
            q->itemsIt.type.state &= ~(END|PROCESSING|SUCCESS|ERROR);
        }else{
            q->itemsIt.type.state &= ~(END|SUCCESS|ERROR);
        }

        i32 idx = q->itemsIt.idx;
        if(q->itemsIt.type.state & PROCESSING){
            idx += SPAN_STRIDE;
        }else{
            q->itemsIt.type.state |= PROCESSING;
        }

        while((q->itemsIt.type.state & (SUCCESS|END|ERROR)) == 0){
            Iter_GetByIdx(&q->itemsIt, idx);
            idx += SPAN_STRIDE;
        }

        void **slab = NULL;
        if(p->dims == 0){
            slab = (void **)p->root; 
        }else{
            slab = q->itemsIt.stack[1];
        }

        i32 slabIdx = q->itemsIt.idx / SPAN_STRIDE;

        Iter it;
        Iter_Init(&it, q->crit.handlers);
        while((Iter_Next(&it) & END) == 0){
            QueueCrit *crit = (QueueCrit *)Iter_Get(&it);
            void **dataSlab = Span_Get(q->crit.data, slabIdx);
            q->go |= crit->func(crit, slab, dataSlab);
        }
    }

    if(q->go && (q->itemsIt.type.state & SUCCESS)){
        while(q->localIdx < SPAN_STRIDE){
            if(q->go & (1 << q->localIdx)){
                void **slab = NULL;
                if(p->dims == 0){
                    slab = (void **)p->root; 
                }else{
                    slab = q->itemsIt.stack[1];
                }

                void **ptr = (void **)q->itemsIt.stack[0];
                if(ptr != NULL){
                    q->value = *ptr;
                    q->type.state |= SUCCESS;
                }else{
                    q->value = NULL;
                    q->type.state |= NOOP;
                }
                q->localIdx++;
                break;
            }
            q->localIdx++;
        }
    }

    return q->type.state;
}

status Queue_AddHandler(Queue *q, QueueCrit *crit){
    status r = READY;
    r |= Span_Add(q->crit.handlers, (Abstract *)crit);
    MemCh *m = Queue_GetMem(q);
    r |= Span_Add(q->crit.data, (Abstract *)Iter_Make(m, Span_Make(m)));
    return r;
}

Queue *Queue_Make(MemCh *m){
    Queue *q = MemCh_AllocOf(m, sizeof(Queue), TYPE_QUEUE);
    q->type.of = TYPE_QUEUE;
    Iter_Init(&q->itemsIt, Span_Make(m));
    q->available = Span_Make(m);
    q->crit.handlers = Span_Make(m);
    q->crit.data = Span_Make(m);
    return q;
}
