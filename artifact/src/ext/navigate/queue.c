#include <external.h>
#include <caneka.h>

static gobits Queue_CheckSlab(Queue *q);

status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, void *value){
    status r = READY;
    i32 slabIdx = idx / SPAN_STRIDE;
    MemCh *m = Queue_GetMem(q);
    void **slab = NULL;
    boolean half = (q->type.state & QUEUE_CRIT_HALF) != 0; 
    if((slab = Span_Get(q->crit.data, slabIdx)) == NULL){
        size_t sz = (half ? sizeof(word) : sizeof(util));
        slab = MemCh_Alloc(m, sizeof(sz*SPAN_STRIDE); 
        r |= Span_Set(q->crit.data, slabIdx, (Abstract *)slab);
    } 
    i32 localIdx = idx-slabIdx*SPAN_STRIDE;
    if(half){
        void *hslab = (word *)
        hslab[localIdx] = *(word *)value;
        r |= SUCCESS;
    }else{
        void *uslab = (util *)
        uslab[localIdx] = *(util *)value;
        r |= SUCCESS;
    }
    return r;
}

status Queue_Next(Queue *q){
    ;
}

status Queue_AddHandler(Queue *q, QueueCrit *crit){
    status r = READY;
    r |= Span_Add(q->crit.handlers, (Abstract *)crit);
    r |= Span_Add(q->crit.data, (Abstract *)Iter_Make(Queue_GetMem(q)));
    return r;
}

Queue *Queue_Make(MemCh *m){
    Queue *q = MemCh_AllocOf(m, sizeof(Queue));
    q->type.of = TYPE_QUEUE;
    Iter_Init(&q->itemsIt, Span_Make(m));
    q->available = Span_Make(m);
    q->crit.handlers = Span_Make(m);
    q->crit.data = Span_Make(m);
    return q;
}
