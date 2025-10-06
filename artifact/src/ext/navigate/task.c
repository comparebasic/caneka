#include <external.h>
#include <caneka.h>

status Task_Tumble(Task *tsk){
    do {
        Step *st = Iter_Current(&tsk->chainIt);
        st->func(st, tsk);
    } while(tsk->type.state & MORE);
    return tsk->type.state;
}

status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source){
    return Iter_AddOn(&tsk->chainIt, (Abstract *)Step_Make(tsk->m, func, arg, source));
}

Task *Task_Make(Span *chain){
    MemCh *m = MemCh_Make();
    Task *tsk = MemCh_AllocOf(m, sizeof(Task), TYPE_TASK);
    tsk->type.of = TYPE_TASK;
    Iter_Init(&tsk->chainIt, chain);
    return tsk;
}
