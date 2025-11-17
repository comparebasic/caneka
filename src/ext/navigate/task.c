#include <external.h>
#include <caneka.h>

static MemCh *_currentTask_m = NULL;

Table *TaskErrorHandlers = NULL; /* TaskPopulate */

static status _taskErrorHandler(MemCh *m, void *_tsk, void *msg){
    void *args[5];
    Task *tsk = (Task *)as(_tsk, TYPE_TASK);
    Single *key = Util_Wrapped(m, (util)tsk->parent);
    Single *funcW = (Single *)Table_Get(TaskErrorHandlers, key);
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
    i16 guard = 0;
    microTime start = MicroTime_Now();
    do {
        if(tsk->type.state & TASK_CHECK_ELAPSED
            && MicroTime_Now() - tsk->metrics.start > tsk->timeout){
                Error(tsk->m, FUNCNAME, FILENAME, LINENUMBER, 
                    "Task timed out", NULL);
            break;
        }
        if(!Guard(&guard, tsk->stepGuardMax, FUNCNAME, FILENAME, LINENUMBER)){
            Error(tsk->m, FUNCNAME, FILENAME, LINENUMBER,
                "Guard max exceeded for task", NULL);
            break;
        }
        tsk->type.state &= ~(MORE|NOOP);

        Step *st = Iter_Get(&tsk->chainIt);
        if(st->type.state & ERROR){
            break;
        }

        if(tsk->type.state & DEBUG){
            void *args[] = {
                Util_Wrapped(OutStream->m, (util)st->func),
                tsk,
                NULL,
            };
            Out("^y.Tumble -> ^D.$,^d. of &^0\n", args);
        }

        status r = READY;
        boolean ran = FALSE;
        if((st->type.state & (SUCCESS|ERROR)) == 0){
            r |= PROCESSING;
            Guard_Incr(tsk->m, &st->g, tsk->stepGuardMax, FUNCNAME, FILENAME, LINENUMBER);
            r = st->func(st, tsk);
            ran = TRUE;
        }

        if(tsk->type.state & TASK_UPDATE_CRIT){
            if(tsk->parent != NULL){
                printf("updating crit for %d\n", tsk->idx);
                fflush(stdout);
                Queue_SetCriteria((Queue *)tsk->parent->data, 0, tsk->idx, &tsk->u);
            }
            tsk->type.state &= ~TASK_UPDATE_CRIT;
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


        if((tsk->type.state & (MORE|TASK_CHILD)) == MORE){
            Queue *q = (Queue *)as(tsk->data, TYPE_QUEUE);
            printf("MORE %d noop?%d q->nvalues:%d\n",
                tsk->type.state, tsk->type.state & (NOOP|PROCESSING), q->it.p->nvalues);
            ErrA = (Abstract *)q;
        }

    } while((tsk->type.state & (MORE|ERROR)) == MORE);

    if((tsk->chainIt.type.state & END) && tsk->chainIt.p->nvalues == 0){
        tsk->type.state |= SUCCESS;
    }

    tsk->metrics.consumed = MicroTime_Now() - start;
    DebugStack_Pop();
    return tsk->type.state;
}

status Task_ResetChain(Task *tsk){
    Iter_Init(&tsk->chainIt, Span_Make(tsk->m));
    return SUCCESS;
}

status Task_AddDataStep(Task *tsk, StepFunc func, void *arg, void *data, void *source, word flags){
    Step *st = Step_Make(tsk->m, func, arg, source, flags);
    st->data = data;
    status r = Iter_Add(&tsk->chainIt, st);
    tsk->type.state |= MORE;
    return r|MORE;
}

status Task_AddStep(Task *tsk, StepFunc func, void *arg, void *source, word flags){
    return Task_AddDataStep(tsk, func, arg, NULL, source, flags);
}

Task *Task_Make(Span *chain, void *source){
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
    tsk->metrics.start = MicroTime_Now();
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
