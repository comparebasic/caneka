#include <external.h>
#include <caneka.h>

Table *TaskErrorHandlers = NULL; /* TaskPopulate */

static status _taskErrorHandler(MemCh *m, Abstract *_tsk, Abstract *msg){
    Abstract *args[5];
    Task *tsk = (Task *)as(_tsk, TYPE_TASK);
    Single *key = Util_Wrapped(m, (util)tsk->parent);
    Single *funcW = (Single *)Table_Get(TaskErrorHandlers, (Abstract *)key);
    if(funcW != NULL){
        TaskPopulate func = (TaskPopulate)funcW->val.ptr;
        func(m, tsk, msg, tsk->source);
        return NOOP;
    }
    return ERROR;
}

status Task_Tumble(Task *tsk){
    DebugStack_Push(tsk, tsk->type.of);
    tsk->type.state &= ~SUCCESS;
    i16 guard;
    do {
        if(Guard_Incr(tsk->m, &guard, tsk->stepGuardMax, FUNCNAME, FILENAME, LINENUMBER)
                & ERROR){
            break;
        }
        if(tsk->type.state & TASK_UPDATE_CRIT){
            if(tsk->parent != NULL){
                Queue_SetCriteria((Queue *)tsk->parent->data, 0, tsk->idx, &tsk->u);
                tsk->type.state &= ~TASK_UPDATE_CRIT;
            }
        }
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
            Guard_Incr(tsk->m, &st->g, tsk->stepGuardMax, FUNCNAME, FILENAME, LINENUMBER);
            r = st->func(st, tsk);
        }
        if(((r & MORE) == 0)){
            if(st->type.state & SUCCESS){
                Iter_Remove(&tsk->chainIt);
                Iter_Prev(&tsk->chainIt);
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
    } while((tsk->type.state & (MORE|ERROR)) == MORE);

    if((tsk->chainIt.type.state & END) && tsk->chainIt.p->nvalues == 0){
        tsk->type.state |= SUCCESS;
    }

    DebugStack_Pop();
    return tsk->type.state;
}

status Task_ResetChain(Task *tsk){
    Iter_Init(&tsk->chainIt, Span_Make(tsk->m));
    return SUCCESS;
}

status Task_AddDataStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *data, Abstract *source, word flags){
    Step *st = Step_Make(tsk->m, func, arg, source, flags);
    st->data = data;
    status r = Iter_Add(&tsk->chainIt, (Abstract *)st);
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
    tsk->stepGuardMax = TASK_TUMPLE_MAX;
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
