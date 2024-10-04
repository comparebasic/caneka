#include <external.h>
#include <caneka.h>

static status Recieve(Serve *sctx, Req *req){
    printf("Recieving\n");
    req->type.state |= NEXT;
    return SUCCESS;
}
static status Respond(Serve *sctx, Req *req){
    printf("Responding\n");
    req->type.state |= NEXT;
    return SUCCESS;
}

static void *handlers[] = {
   Recieve, Respond, NULL 
};

Handler Example_getHandlers(Serve *sctx, Req *req){
    return (Handler) handlers;
}
