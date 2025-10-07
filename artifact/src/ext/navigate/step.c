#include <external.h>
#include <caneka.h>

Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source, word flags){
    Step *st = MemCh_AllocOf(m, sizeof(Step), TYPE_STEP);
    st->type.of = TYPE_STEP;
    st->type.state = flags;
    st->func = func;
    st->arg = arg;
    st->source = source;
    return st;
}
