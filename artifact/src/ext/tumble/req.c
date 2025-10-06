#include <external.h>
#include <caneka.h>

status Req_Run(Req *req){
    do {
        req->chain->arg = Handler(req->m, req->chain, req);
    }(while(
        (req->chain->type.state & (SUCCESS|ERROR)) == 0 
            && req->chain->type.state & MORE);
    return req->chain->type.state;
}

Req *Req_Make(Handler *chain, Abstract *source){
    MemCh *m = MemCh_Make();
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_TUMBLE_REQ);
    req->type.of = TYPE_TUMBLE_REQ;
    req->chain = chain;
    req->source = source;
    return req;
}
