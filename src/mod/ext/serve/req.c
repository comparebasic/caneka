#include <external.h>
#include <caneka.h>

Req *Req_Make(MemCh *m){
    Req *core = MemCh_AllocOf(m, sizeof(Req), TYPE_TASK_CORE);
    core->type.of = TYPE_REQ;
    return core;
}
