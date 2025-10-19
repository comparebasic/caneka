#include <external.h>
#include <caneka.h>

static gobits Queue_CheckSlab(Queue *q);

i32 Queue_GetIdx(Queue *q){
    return q->itemsIt.idx;
}

status Queue_Remove(Queue *q, i32 idx){
    status r = READY;
    r |= Span_Remove(q->itemsIt.p, idx);
    util u = 0;
    r |= Queue_SetCriteria(q, 0, idx, &u);
    return r;
}

i32 Queue_Add(Queue *q, Abstract *a, util *crit){
    i32 idx = q->itemsIt.p->max_idx+1;
    Abstract *args[4];
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(OutStream->m, idx);
        args[1] = (Abstract *)a;
        args[2] = (Abstract *)q;
        args[3] = NULL;
        Out("^p.Queue_Add \\@$/@ &\n", args);
    }
    Queue_Set(q, idx, a);
    Queue_SetCriteria(q, 0, idx, crit);
    return idx;
}

status Queue_Set(Queue *q, i32 idx, Abstract *a){
    status r = READY;
    r |= Span_Set(q->itemsIt.p, idx, a);
    if(q->itemsIt.type.state & MORE){
        Iter_GetByIdx(&q->itemsIt, q->itemsIt.idx);
        r |= MORE;
    }
    return r;
}

status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, util *value){
    status r = READY;
    Abstract *args[4];
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(OutStream->m, idx);
        args[1] = (Abstract *)Str_Ref(OutStream->m, 
            (byte *)value, sizeof(util), sizeof(util), STRING_BINARY);
        args[2] = (Abstract *)q;
        args[3] = NULL;
        Out("^p.Queue_SetCriteria \\@$/& &\n", args);
    }
    i32 slabIdx = idx / CRIT_SLAB_STRIDE;
    MemCh *m = Queue_GetMem(q);
    void *slab = NULL;
    QueueCrit *crit = Span_Get(q->handlers, critIdx);
    if(crit == NULL){
        args[0] = (Abstract *)I32_Wrapped(ErrStream->m, critIdx);
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Criteria object is null when adding criteria of critIdx $", args);
        return ERROR;
    }
    if((slab = Span_Get(crit->data, slabIdx)) == NULL){
        slab = 
            (void *)Bytes_Alloc(m, sizeof(util)*CRIT_SLAB_STRIDE, TYPE_BYTES_POINTER);
        r |= Span_Set(crit->data, slabIdx, (Abstract *)slab);
    } 
    i32 localIdx = idx-(slabIdx*CRIT_SLAB_STRIDE);
    util *uslab = (util *)slab;
    uslab[localIdx] = *((util *)value);
    r |= SUCCESS;
    return r;
}

static status Queue_SetGo(Queue *q){

    return q->go ? SUCCESS : NOOP;
}

status Queue_Reset(Queue *q){
    Iter_Init(&q->itemsIt, q->itemsIt.p);
    q->go = 0;
    q->slabIdx = -1;
    q->type.state = q->type.state & DEBUG;
    return SUCCESS;
}

status Queue_Next(Queue *q){
    Abstract *args[5];
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)q;
        args[1] = NULL;
        Out("^p.Queue_Next &\n", args);
    }
    if(q->type.state & END){
        Queue_Reset(q);
    }
    q->type.state &= ~(SUCCESS|NOOP|END);
    q->value = NULL;
    if(q->itemsIt.p->nvalues == 0){
        q->type.state |= END;
        return q->type.state;
    }

    while((Iter_Next(&q->itemsIt) & END) == 0){
        Iter it;
        if(q->itemsIt.idx >= (q->slabIdx+1)*CRIT_SLAB_STRIDE){
            q->slabIdx++;
            q->go = 0;
            Iter_Init(&it, q->handlers);
            while((Iter_Next(&it) & END) == 0){
                QueueCrit *crit = (QueueCrit *)Iter_Get(&it);
                util *slab = (util *)Span_Get(crit->data, q->slabIdx);
                q->go |= crit->func(crit, slab);
            }
        }

        i32 localIdx = (q->itemsIt.idx & CRIT_SLAB_MASK); 
        if(q->go & (1 << localIdx)){
            q->type.state |= SUCCESS;
            q->value = Iter_Get(&q->itemsIt);
            if(q->type.state & DEBUG){
                args[0] = (Abstract *)I32_Wrapped(OutStream->m, q->itemsIt.idx);
                args[1] = (Abstract *)q->value;
                args[2] = NULL;
                Out("^p.    Found \\@$/&\n", args);
            }
            break;
        }
    }

    q->type.state |= (q->itemsIt.type.state & END);
    return q->type.state;
}

status Queue_AddHandler(Queue *q, QueueCrit *crit){
    status r = READY;
    r |= Span_Add(q->handlers, (Abstract *)crit);
    return r;
}

Queue *Queue_Make(MemCh *m){
    Queue *q = MemCh_AllocOf(m, sizeof(Queue), TYPE_QUEUE);
    q->type.of = TYPE_QUEUE;
    Iter_Init(&q->itemsIt, Span_Make(m));
    q->available = Span_Make(m);
    q->handlers = Span_Make(m);
    q->slabIdx = -1;
    return q;
}
