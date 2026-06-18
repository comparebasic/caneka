#include <external.h>
#include <caneka.h>

Req *Make_Req(MemCh *m){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_REQ);
    req->type.of = TYPE_REQ;
    return req;
}
