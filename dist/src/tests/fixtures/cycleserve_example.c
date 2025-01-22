#include <external.h>
#include <caneka.h>

status Example_Setup(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Setup: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    h->type.state |= SUCCESS;
    return h->type.state;
}

status Example_Recieve(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Recieving: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }

    status r = Req_Recv(sctx, req);
    h->type.state |= (r & (SUCCESS|ERROR));
    if(DEBUG_EXAMPLE_HANDLERS){
        printf("\x1b[%dmRecieved %s\x1b[0m\n", DEBUG_EXAMPLE_HANDLERS, State_ToChars(h->type.state));
    }
    return h->type.state;
}

status Example_Process(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Processing: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    String *response = String_Init(req->m, STRING_EXTEND);
    String_AddBytes(req->m, response, bytes("Hi, you requested: "), strlen("Hi, you requested: "));
    String_Add(req->m, response, ((HttpProto *)req->proto)->path);

    h->type.state |= Req_SetResponse(req, 200, NULL, response);
    
    return h->type.state;
}

status Example_Respond(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Responding: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    h->type.state |= Req_Respond(sctx, req) & (SUCCESS|ERROR);
    return h->type.state;
}

status Example_Complete(Handler *h, Req *req, Serve *sctx){
    if(DEBUG_EXAMPLE_HANDLERS){
        Debug_Print((void *)req, 0, "Complete: ", DEBUG_EXAMPLE_HANDLERS, FALSE);
        printf("\n");
    }
    h->type.state |= SUCCESS;
    req->type.state |= END|SUCCESS;
    return h->type.state;
}

Handler *Example_getHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    Handler *startHandler = Handler_Make(m, Example_Complete, NULL, SOCK_OUT|SOCK_IN);
    startHandler->prior = Span_Make(m, TYPE_SPAN); 
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Example_Setup, NULL, SOCK_IN));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Example_Recieve, NULL, SOCK_IN));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Example_Process, NULL, SOCK_OUT));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Example_Respond, NULL, SOCK_OUT));
    Span_Add(startHandler->prior, (Abstract *)Handler_Make(m, Example_Complete, NULL, SOCK_OUT));

    return startHandler;
}
