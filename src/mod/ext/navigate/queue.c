#include <external.h>
#include <caneka.h>

i32 Queue_GetIdx(Queue *q){
    return q->it.idx;
}

status Queue_Remove(Queue *q, i32 idx){
    status r = READY;
    Iter it;
    Iter_Init(&it, q->it.p);
    Iter_RemoveByIdx(&it, idx);
    q->type.state |= (it.type.state & (FLAG_ITER_REVERSE|END));
    MemCh *m = q->it.p->m;

    Iter_Init(&it, q->handlers);
    while((Iter_Next(&it) & END) == 0){
        util *uptr = Queue_GetCriteria(q, it.idx, idx);
        *uptr = 0;
    }

    Single *sg = NULL;
    if(Shelf_Available(&q->availableIt)){
        sg = (Single *)Shelf_Get(&q->availableIt);
    }else{
        sg = I32_Wrapped(m, idx);
        Shelf_Add(&q->availableIt, sg);
    }
    sg->val.i = idx;

    return q->type.state;
}

i32 Queue_Add(Queue *q, void *a){
    i32 idx = 0;
    Single *sg = NULL;
    if((sg = (Single *)Shelf_Get(&q->availableIt)) != NULL){
        idx = sg->val.i;
    }else{
        idx = q->it.p->max_idx+1;
    }

    MemCh *m = q->it.p->m;
    Queue_Set(q, idx, a);
    return idx;
}

status Queue_Set(Queue *q, i32 idx, void *a){
    q->type.state &= ~(FLAG_ITER_REVERSE|END);
    status r = READY;
    Iter it;
    Iter_Init(&it, q->it.p);
    r |= Iter_SetByIdx(&it, idx, a);
    Iter_GetByIdx(&q->it, q->it.idx);

    return r;
}

util *Queue_GetCriteria(Queue *q, i32 critIdx, i32 idx){
    Iter it;
    Iter_Init(&it, q->handlers);
    QueueCrit *crit = (QueueCrit *)Span_Get(q->handlers, critIdx);
    return Span_GetSlot(crit->data, idx);
}

status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, util **value){
    status r = READY;
    void *args[2];
    MemCh *m = q->it.p->m;
    util *slot = NULL;
    QueueCrit *crit = Span_Get(q->handlers, critIdx);

    if(crit == NULL){
        args[0] = I32_Wrapped(ErrStream->m, critIdx);
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Criteria object is null when adding criteria of critIdx $", 
            args);
        return ERROR;
    }

    util *u = Span_GetSlot(crit->data, idx);
    if(u == NULL){
        u = Span_SetSlot(crit->data, idx, 0);
    } 

    *value = u;
    r |= SUCCESS;
    return r;
}

status Queue_Reset(Queue *q){
    Iter_Init(&q->it, q->it.p);
    q->slabIdx = -1;
    q->type.state = q->type.state & DEBUG;
    return SUCCESS;
}

status Queue_Next(Queue *q){
    if(q->type.state & END){
        Queue_Reset(q);
    }

    MemCh *m = q->it.p->m;
    q->type.state &= ~(SUCCESS|NOOP|END);
    q->value = NULL;
    if(q->it.p->nvalues == 0){
        q->type.state |= END;
        return q->type.state;
    }

    while((Iter_Next(&q->it) & END) == 0){
        Iter it;
        Iter_Init(&it, q->handlers);
        while((Iter_Next(&it) & END) == 0){
            QueueCrit *crit = (QueueCrit *)Iter_Get(&it);
            util *slab = (util *)Span_Get(crit->data, q->slabIdx);

            crit->type.state |= (q->type.state & DEBUG);
            if(slab != NULL){
                q->go |= crit->func(crit, slab);
                /*
                Bits_Print(OutStream, (byte *)&q->go, sizeof(util), MORE|DEBUG);
                Buff_AddBytes(OutStream, (byte *)"\n", 1); 
                */
            }
        }
    }

    q->type.state |= (q->it.type.state & END);
    return q->type.state;
}

i32 Queue_AddHandler(Queue *q, QueueCrit *crit){
    Span_Add(q->handlers, crit);
    return q->handlers->max_idx;
}

Queue *Queue_Make(MemCh *m){
    Queue *q = MemCh_AllocOf(m, sizeof(Queue), TYPE_QUEUE);
    q->type.of = TYPE_QUEUE;
    Iter_Init(&q->it, Span_Make(m));
    Iter_Init(&q->availableIt, Span_Make(m));
    q->handlers = Span_Make(m);
    q->slabIdx = -1;
    return q;
}
