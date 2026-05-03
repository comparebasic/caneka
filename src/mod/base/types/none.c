#include <external.h>
#include "base_module.h"

None *None_Make(MemCh *m, status state){
    None *n = MemCh_AllocOf(m, sizeof(None), TYPE_NONE);
    n->type.of = TYPE_NONE;
    n->type.state = state;
    return n;
}
