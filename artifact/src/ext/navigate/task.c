#include <external.h>
#include <caneka.h>

status Task_Free(Step *st, Task *tsk){
    return MemCh_Free(tsk->m);
}

status Task_Tumble(Task *tsk){
    DebugStack_Push(tsk, tsk->type.of);
    tsk->type.state &= ~SUCCESS;
    do {
        tsk->type.state &= ~MORE;
        Step *st = Iter_Current(&tsk->chainIt);
        if(tsk->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)Util_Wrapped(OutStream->m, (util)st->func),
                (Abstract *)tsk,
                NULL,
            };
            Out("^y.Tumble -> ^D.$,^d. of &^0\n", args);
        }
        status r = READY;
        if((st->type.state & (SUCCESS|ERROR)) == 0){
            r |= PROCESSING;
            r = st->func(st, tsk);
        }
        if(((r & MORE) == 0)){
            if(st->type.state & SUCCESS){
                Iter_PrevRemove(&tsk->chainIt);
            }else if(st->type.state & STEP_LOOP){
                i32 idx = tsk->chainIt.p->max_idx;
                Step *tail = Span_Get(tsk->chainIt.p, idx);
                if((tail->type.state & (SUCCESS|ERROR)) == 0){
                    Iter_GetByIdx(&tsk->chainIt, idx);
                }else{
                    Iter_Prev(&tsk->chainIt);
                }
            }else if(st->type.state & NOOP){
                Iter_Prev(&tsk->chainIt);
            }

            if((tsk->chainIt.type.state & END) == 0){
               tsk->type.state |= MORE; 
            }
        }
    } while(tsk->type.state & MORE);

    if((tsk->chainIt.type.state & END) && tsk->chainIt.p->nvalues == 0){
        tsk->type.state |= SUCCESS;
    }

    DebugStack_Pop();
    return tsk->type.state;
}

status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source, word flags){
    if(tsk->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)tsk,
            (Abstract *)Util_Wrapped(OutStream->m, (util)func),
            (Abstract *)args,
            (Abstract *)source,
            (Abstract *)Word_Wrapped(OutStream->m, (word)flags),
            NULL,
        };
        Out("^y.AddStep($, ^D.$,^d., @, @, $)^0\n", args);
    }
    Step *st = Step_Make(tsk->m, func, arg, source, flags);
    status r = Iter_AddOn(&tsk->chainIt, (Abstract *)st);
    tsk->type.state |= MORE;
    return r|MORE;
}

Task *Task_Make(Span *chain, Abstract *source){
    MemCh *m = NULL;
    if(chain == NULL){
        m = MemCh_Make();
        chain = Span_Make(m);
    }else{
        m = chain->m;
    }
    Task *tsk = MemCh_AllocOf(m, sizeof(Task), TYPE_TASK);
    tsk->type.of = TYPE_TASK;
    tsk->m = m;
    Iter_Init(&tsk->chainIt, chain);
    tsk->source = source;
    return tsk;
}
