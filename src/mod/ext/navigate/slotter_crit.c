#include <external.h>
#include <caneka.h>

SlotterCrit *SlotterCrit_Make(MemCh *m, SlotterFunc func, word flags){
    SlotterCrit *crit = MemCh_AllocOf(m, sizeof(SlotterCrit), TYPE_SLOTTER_CRIT);
    crit->type.of = TYPE_SLOTTER_CRIT;
    crit->type.state = flags;
    crit->func = func;
    crit->data = Span_Make(m);
    return crit;
}
