#include <external.h>
#include <caneka.h>

status Task_Tumble(Task *tsk){
    tsk->type.state &= ~SUCCESS;
    do {
        tsk->type.state &= ~MORE;
        Step *st = Iter_Current(&tsk->chainIt);
        status r = READY;
        if((st->type.state & (SUCCESS|ERROR|NOOP)) == 0){
            r |= PROCESSING;
            r = st->func(st, tsk);
        }
        if(((r & MORE) == 0)){
            if(st->type.state & SUCCESS){
                Iter_PrevRemove(&tsk->chainIt);
            if(st->type.state & NOOP){
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

    return tsk->type.state;
}

status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source, word flags){
    Step *st = Step_Make(tsk->m, func, arg, source, flags);
    status r = Iter_AddOn(&tsk->chainIt, (Abstract *)st);
    tsk->type.state |= MORE;
    return r|MORE;
}

Task *Task_Make(Span *chain, Abstract *source){
    MemCh *m = MemCh_Make();
    Task *tsk = MemCh_AllocOf(m, sizeof(Task), TYPE_TASK);
    tsk->type.of = TYPE_TASK;
    tsk->m = m;
    Iter_Init(&tsk->chainIt, chain);
    tsk->source = source;
    return tsk;
}
