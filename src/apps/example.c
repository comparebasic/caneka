#include <external.h>
#include <caneka.h>

static status Setup(Serve *sctx, Req *req){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Setup: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    req->direction = EPOLLIN;
    req->type.state |= NEXT;
    return SUCCESS;
}

static status Recieve(Serve *sctx, Req *req){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Recieving: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    req->type.state |= NEXT;
    req->direction = EPOLLOUT;
    return SUCCESS;
}

static status Respond(Serve *sctx, Req *req){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Responding: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    req->type.state |= NEXT;
    return SUCCESS;
}

static Handler handlers[] = {
   Setup, Recieve, Respond, NULL 
};

Handler *Example_getHandlers(Serve *sctx, Req *req){
    return handlers;
}
