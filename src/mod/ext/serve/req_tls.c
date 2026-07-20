#include <external.h>
#include <caneka.h>

status ReqTls_Read(MemCh *m, ReqHttp *req, Serve *srv){
    if((req->rbl->type.state & (SUCCESS|ERROR)) == 0){
        srv->capsule->readTo(req->cap);
    }
}
