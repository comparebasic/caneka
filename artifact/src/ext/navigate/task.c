#include <external.h>
#include <caneka.h>

Table *TaskErrorHandlers = NULL;

static status _taskErrorHandler(MemCh *m, Abstract *_tsk){
    Abstract *args[5];
    Task *tsk = (Task *)as(_tsk, TYPE_TASK);
    tsk->type.state |= ERROR;
    args[0] = (Abstract *)tsk;
    args[1] = NULL;
    Out("^r.Error Handling @^0\n", args);
    return NOOP;
}

status Task_Tumble(Task *tsk){
    DebugStack_Push(tsk, tsk->type.of);
    tsk->type.state &= ~SUCCESS;
    do {
        if(tsk->type.state & TASK_UPDATE_CRIT){
            if(tsk->source != NULL && tsk->source->type.of == TYPE_TASK){
                Task *parent = (Task *)tsk->source; 
                Queue_SetCriteria((Queue *)parent->data, 0, tsk->idx, &tsk->u);
            }
        }
        tsk->type.state &= ~(MORE|TASK_UPDATE_CRIT);

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
            Guard_Incr(&st->g, tsk->stepGuardMax, FUNCNAME, FILENAME, LINENUMBER);
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

status Task_AddDataStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *data, Abstract *source, word flags){
    Step *st = Step_Make(tsk->m, func, arg, source, flags);
    st->data = data;
    status r = Iter_AddOn(&tsk->chainIt, (Abstract *)st);
    tsk->type.state |= MORE;
    return r|MORE;
}

status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source, word flags){
    return Task_AddDataStep(tsk, func, arg, NULL, source, flags);
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

status Task_Init(MemCh *m){
    status r = READY;
    if(TaskErrorHandlers == NULL){
        TaskErrorHandlers = Table_Make(m);
        r |= SUCCESS;
    }
    r |= Lookup_Add(m, ErrorHandlers, TYPE_TASK, (void *)_taskErrorHandler);
    return r;
}
