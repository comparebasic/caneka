#include <external.h>
#include <caneka.h>

static gobits Queue_CheckSlab(Queue *q);

i32 Queue_GetIdx(Queue *q){
    return q->itemsIt.idx + q->localIdx;
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
    i32 slabIdx = idx / SPAN_STRIDE;
    MemCh *m = Queue_GetMem(q);
    void *slab = NULL;
    QueueCrit *crit = Span_Get(q->handlers, critIdx);
    if(crit == NULL){
        args[0] = (Abstract *)I32_Wrapped(ErrStream->m, critIdx);
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)q, FUNCNAME, FILENAME, LINENUMBER,
            "Criteria object is null when adding criteria of critIdx $", args);
        return ERROR;
    }
    if((slab = Span_Get(crit->data, slabIdx)) == NULL){
        slab = 
            (void *)Bytes_Alloc(m, sizeof(util)*SPAN_STRIDE, TYPE_BYTES_POINTER);
        r |= Span_Set(crit->data, slabIdx, (Abstract *)slab);
    } 
    i32 localIdx = idx-slabIdx*SPAN_STRIDE;
    util *uslab = (util *)slab;
    uslab[localIdx] = *((util *)value);
    r |= SUCCESS;
    return r;
}

status Queue_Next(Queue *q){
    Span *p = q->itemsIt.p;
    Abstract *args[5];
    if(q->type.state & DEBUG){
        args[0] = (Abstract *)q;
        args[1] = NULL;
        Out("^p.Queue_Next &\n", args);
    }
    MemCh *m = Queue_GetMem(q);
    q->type.state &= ~(SUCCESS|NOOP|END);
    if(p->nvalues == 0){
        q->type.state |= END;
    }else{
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
                q->itemsIt.type.state |= DEBUG;
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
            Iter_Init(&it, q->handlers);
            while((Iter_Next(&it) & END) == 0){
                QueueCrit *crit = (QueueCrit *)Iter_Get(&it);
                void **dataSlab = Span_Get(crit->data, slabIdx);
                q->go |= crit->func(crit, (Abstract **)slab, (util *)dataSlab);
            }

            if(it.type.state & END && q->go == 0){
                q->type.state |= END;
            }
        }

        if(q->go && (q->itemsIt.type.state & SUCCESS)){
            while(q->localIdx < SPAN_STRIDE){
                if(q->go & (1 << q->localIdx)){
                    void **ptr = NULL;;
                    if(p->dims == 0){
                        ptr = (void **)q->itemsIt.p->root;
                    }else{
                        ptr = *((void **)q->itemsIt.stack[1]);
                    }

                    if(ptr != NULL){
                        ptr += q->localIdx;
                        q->value = *ptr;
                        q->type.state |= SUCCESS;
                    }else{
                        q->value = NULL;
                        q->type.state |= NOOP;
                    }
                    break;
                }
                q->localIdx++;
            }
        }
    }

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
    return q;
}
