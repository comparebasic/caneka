#include <external.h>
#include <caneka.h>

TemplCrit *TemplCrit_Make(MemCh *m, i32 idx, status positive, status negative){
    TemplCrit *crit = MemCh_AllocOf(m, sizeof(TemplCrit), TYPE_TEMPL_JUMP_CRIT);
    crit->type.of = TYPE_TEMPL_JUMP_CRIT;
    crit->type.state = positive & NORMAL_FLAGS;
    crit->dflag.positive = positive & UPPER_FLAGS;
    crit->dflag.negative = negative;
    crit->contentIdx = idx;
    crit->dataIdx = -1;
    return crit;
}
