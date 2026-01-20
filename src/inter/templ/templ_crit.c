#include <external.h>
#include <caneka.h>

TemplCrit *TemplCrit_Make(MemCh *m, i32 idx, status flags){
    TemplCrit *crit = MemCh_AllocOf(m, sizeof(TemplCrit), TYPE_TEMPL_JUMP_CRIT);
    crit->type.of = TYPE_TEMPL_JUMP_CRIT;
    crit->type.state = flags;
    crit->contentIdx = idx;
    return crit;
}
