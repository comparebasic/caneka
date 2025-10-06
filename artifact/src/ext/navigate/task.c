#include <external.h>
#include <caneka.h>

status Task_Tumble(Task *tsk){
    do {
        Step *st = Iter_Current(&tks->chainIt);
        st->func(st, tsk);
    }(while(tsk->type.state & MORE);
    return tsk->type.state;
}

Task *Task_Make(Span *chain){
    MemCh *m = MemCh_Make();
    Task *tsk = MemCh_AllocOf(m, sizeof(Task), TYPE_TASK);
    tsk->type.of = TYPE_TUMBLE_REQ;
    Iter_Init(&tsk->chainIt, chain);
    return tsk;
}
