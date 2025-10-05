#include <external.h>
#include <caneka.h>

status Req_Run(Serve *ctx, Req *req){
    req->chain->arg = Handler(req->m, req->chain, req);
    return req->chain->type.state;
}
