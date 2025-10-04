#include <external.h>
#include <caneka.h>

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
