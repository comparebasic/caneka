#include <external.h>
#include <caneka.h>

status Req_Run(Serve *ctx, Req *req){
    while((req->chain->type.state & NORMAL_FLAGS) == 0){ 
        Handler *h = Handler_Get(req->chain);
        if(h == NULL){
            break;
        }
        h->func(h);
    }
    return req->chain->type.state;;
}

