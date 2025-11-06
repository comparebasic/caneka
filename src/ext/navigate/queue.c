#include <external.h>
#include <caneka.h>

static gobits Queue_CheckSlab(Queue *q);

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

    i64 n = -1;
    Iter_Init(&it, q->handlers);
    while((Iter_Next(&it) & END) == 0){
        r |= Queue_SetCriteria(q, it.idx, idx, (util *)&n);
    }

    Single *sg = NULL;
    if(Shelf_Available(&q->availableIt)){
        sg = (Single *)Shelf_Get(&q->availableIt);
    }else{
        sg = I32_Wrapped(m, idx);
        Shelf_Add(&q->availableIt, (Abstract *)sg);
    }
    sg->val.i = idx;

    return q->type.state;
}

i32 Queue_Add(Queue *q, Abstract *a){
    i32 idx = 0;
    Single *sg = NULL;
    if((sg = (Single *)Shelf_Get(&q->availableIt)) != NULL){
        idx = sg->val.i;
    }else{
        idx = q->it.p->max_idx+1;
    }

    Abstract *args[4];
    MemCh *m = q->it.p->m;
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(m, idx);
        args[1] = (Abstract *)a;
        args[2] = (Abstract *)q;
        args[3] = NULL;
        Out("^p.Queue_Add \\@$/@ @\n", args);
    }
    Queue_Set(q, idx, a);
    return idx;
}

status Queue_Set(Queue *q, i32 idx, Abstract *a){
    q->type.state &= ~(FLAG_ITER_REVERSE|END);

    status r = READY;
    Iter it;
    Iter_Init(&it, q->it.p);
    r |= Iter_SetByIdx(&it, idx, a);
    Iter_GetByIdx(&q->it, q->it.idx);

    return r;
}

status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, util *value){
    status r = READY;
    Abstract *args[4];
    MemCh *m = q->it.p->m;
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(m, idx);
        args[1] = (Abstract *)Str_Ref(m, 
            (byte *)value, sizeof(util), sizeof(util), STRING_BINARY);
        args[2] = (Abstract *)q;
        args[3] = NULL;
        Out("^p.Queue_SetCriteria \\@$/@ @\n", args);
    }
    i32 slabIdx = idx / CRIT_SLAB_STRIDE;
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
    Iter_Init(&q->it, q->it.p);
    q->go = 0;
    q->slabIdx = -1;
    q->type.state = q->type.state & DEBUG;
    return SUCCESS;
}

status Queue_Next(Queue *q){
    Abstract *args[5];
    if(q->type.state & END){
        Queue_Reset(q);
    }

    if(q->type.state & DEBUG){
        args[0] = (Abstract *)q;
        args[1] = NULL;
        Out("^p.Queue_Next @\n", args);
    }

    MemCh *m = q->it.p->m;
    q->type.state &= ~(SUCCESS|NOOP|END);
    q->value = NULL;
    if(q->it.p->nvalues == 0){
        q->type.state |= END;
        return q->type.state;
    }

    while((Iter_Next(&q->it) & END) == 0){
        if(q->type.state & DEBUG){
            args[0] = (Abstract *)I32_Wrapped(m, q->it.idx);
            args[1] = NULL;
            Out("^b.    Queue Iter_Next idx$^0\n", args);
        }
        if(q->handlers->nvalues > 0){
            /* if first in slab set the go values */
            if(q->handlers->nvalues > 0 && q->it.idx >= (q->slabIdx+1)*CRIT_SLAB_STRIDE){
                q->slabIdx++;
                q->go = 0;
                Iter it;
                Iter_Init(&it, q->handlers);
                while((Iter_Next(&it) & END) == 0){
                    QueueCrit *crit = (QueueCrit *)Iter_Get(&it);
                    util *slab = (util *)Span_Get(crit->data, q->slabIdx);
                    if(slab != NULL){
                        q->go |= crit->func(crit, slab);
                        if(q->type.state & DEBUG){
                            args[0] = (Abstract *)Str_Ref(m,
                                (byte *)&q->go, sizeof(word), sizeof(word), STRING_BINARY);
                            args[1] = (Abstract *)I32_Wrapped(m, it.idx);
                            args[2] = NULL;
                            Out("^b.    go is now @ after handler $^0\n", args);
                        }
                    }
                }
            }
        }else{
            q->go = 65535; 
        }

        if(q->type.state & DEBUG){
            args[0] = (Abstract *)Str_Ref(m,
                (byte *)&q->go, sizeof(word), sizeof(word), STRING_BINARY);
            args[1] = NULL;
            Out("^b.    Queue go @\n", args);
        }

        i32 localIdx = (q->it.idx & CRIT_SLAB_MASK); 
        if(q->go & (1 << localIdx)){
            q->type.state |= SUCCESS;
            q->value = Iter_Get(&q->it);
            if(q->type.state & DEBUG){
                args[0] = (Abstract *)I32_Wrapped(m, q->it.idx);
                args[1] = (Abstract *)q->value;
                args[2] = NULL;
                Out("^p.    Found \\@$/&\n", args);
            }
            break;
        }
    }

    q->type.state |= (q->it.type.state & END);
    return q->type.state;
}

i32 Queue_AddHandler(Queue *q, QueueCrit *crit){
    Span_Add(q->handlers, (Abstract *)crit);
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
