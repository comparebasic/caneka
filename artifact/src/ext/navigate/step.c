#include <external.h>
#include <caneka.h>

Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source){
    Step *h = MemCh_AllocOf(m, sizeof(Step), TYPE_STEP);
    h->type.of = TYPE_STEP;
    h->func = func;
    h->arg = arg;
    h->source = source;
    return h;
}
