#include <external.h>
#include <caneka.h>

i32 Queue_Add(Queue *q, void *a, void *crit){
    i32 idx = 0;
    Single *sg = NULL;
    if((sg = (Single *)Shelf_Get(&q->availableIt)) != NULL){
        idx = sg->val.i;
    }else{
        idx = q->it.p->max_idx+1;
    }
    
    Queue_Set(q, idx, a, crit);

    return idx;
}

void Queue_Set(Queue *q, i32 idx, void *a, void *crit){
    i32 prev = q->it.idx;
    Iter_SetByIdx(&q->it, idx, a);
    Iter_GetByIdx(&q->it, prev);

    Iter_SetByIdx(&q->critIt, idx, crit);
}

void Queue_Remove(Queue *q, i32 idx){
    MemCh *m = q->it.p->m;
    Iter_RemoveByIdx(&q->it, idx);
    Iter_RemoveByIdx(&q->critIt, idx);

    Single *sg = NULL;
    if(Shelf_Available(&q->availableIt)){
        sg = (Single *)Shelf_Get(&q->availableIt);
    }else{
        sg = I32_Wrapped(m, idx);
        Shelf_Add(&q->availableIt, sg);
    }
    sg->val.i = idx;
}

void *Queue_GetCriteria(Queue *q, i32 idx){
    i32 prev = q->it.idx;
    void *crit = Iter_GetByIdx(&q->critIt, idx);
    Iter_GetByIdx(&q->critIt, prev);
    return crit;
}

status Queue_Next(Queue *q){
    if(q->it.type.state & END){
        Queue_Reset(q);
    }
    q->type.state |= MORE;

    while((q->type.state & MORE) && (q->it.type.state & END) == 0){
        Iter_Next(&q->it);
        if(q->func != NULL){
            void *crit = Iter_GetByIdx(&q->critIt, q->it.idx);
            if(q->func(q, Iter_Get(&q->it), crit)){
                q->type.state &= ~MORE;
            }else{
                q->type.state |= MORE;
            }
        }
    }

    q->type.state |= q->it.type.state;
    return q->type.state;
}

void Queue_Reset(Queue *q){
    Iter_Init(&q->it, q->it.p);
    Iter_Init(&q->critIt, q->critIt.p);
    q->type.state = q->it.type.state;
}

void *Queue_Get(Queue *q){
    return Iter_Get(&q->it);
}

Queue *Queue_Make(MemCh *m, QueueCritFunc func){
    Queue *q = MemCh_AllocOf(m, sizeof(Queue), TYPE_QUEUE);
    q->type.of = TYPE_QUEUE;
    q->func = func;
    Iter_Init(&q->it, Span_Make(m));
    Iter_Init(&q->availableIt, Span_Make(m));
    Iter_Init(&q->critIt, Span_Make(m));
    ApproxTime_Init(&q->time.delta);
    return q;
}
