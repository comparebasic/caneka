#include <external.h>
#include <caneka.h>

status Step_Add(MemCh *m, Task *tsk, StepFunc func, Abstract *arg, Abstract *source){
    return Iter_AddOn(&tsk->chainIt, (Abstract *)Step_Make(m, func, arg, source));
}

Abstract *Step_Tail(Step *chain){
    while(chain->depends != NULL){
        chain = chain->depends;
    }
    return chain;
}

Abstract *Step_Get(MemCh *m, Step *h, Req *req){
    Step *next = h;
    while(next->depends != NULL){
        next = next->depends;
        if((next->type.state & (SUCCESS|ERROR)) == 0){
            h = next;
        }
    }
    return h;
}

Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source){
    Step *h = MemCh_AllocOf(m, sizeof(Step), TYPE_HANDLER);
    h->func = func;
    h->arg = arg;
    h->source = source;
    return h;
}
