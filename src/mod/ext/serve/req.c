#include <external.h>
#include <caneka.h>

Req *Req_Make(MemCh *m, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_TASK_CORE);
    req->type.of = TYPE_REQ;
    req->source = source;

    return req;
}
