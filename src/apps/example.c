#include <external.h>
#include <caneka.h>

static status Setup(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Setup: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    req->direction = EPOLLIN;
    h->type.state |= SUCCESS;
    return h->type.state;
}

static status Recieve(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Recieving: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    req->direction = EPOLLOUT;
    h->type.state |= SUCCESS;
    return h->type.state;
}

static status Respond(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Responding: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    h->type.state |= SUCCESS;
    return h->type.state;
}

static status Complete(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Complete: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    h->type.state |= SUCCESS;
    return h->type.state;
}

Handler *Example_getHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    Handler *startHandler = Handler_Make(m, Complete, NULL);
    startHandler->prior = Span_Make(m, TYPE_SPAN); 
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Setup, NULL));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Recieve, NULL));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Respond, NULL));

    return startHandler;
}
