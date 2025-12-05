#include <external.h>
#include <caneka.h>

status Step_Delay(Step *st, Task *tsk){
    st->type.state &= ~(NOOP|SUCCESS);
    Single *sg = (Single *)as(st->arg, TYPE_WRAPPED_PTR);
    if(sg->objType.of != TYPE_TIMESPEC){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error objTypeOf was expected to be a TIMESPEC", NULL);
        return ERROR;
    }
    struct timespec *ts = sg->val.ptr;
    struct timespec remaining;
    Time_Delay(ts, &remaining);
    if(tsk->chainIt.idx == tsk->chainIt.p->max_idx){
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= NOOP;
    }
    return st->type.state;
}

Step *Step_Make(MemCh *m, StepFunc func, void *arg, void *source, word flags){
    Step *st = MemCh_AllocOf(m, sizeof(Step), TYPE_STEP);
    st->type.of = TYPE_STEP;
    st->type.state = flags;
    st->func = func;
    st->arg = arg;
    st->source = source;
    return st;
}
