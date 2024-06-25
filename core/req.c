#include <external.h>
#include <filestore.h>

static char *okBase_cstr = "HTTP/1.1 200 OK\r\nServer: filestore\r\nContent-Length: ";
static String *packageResponse(MemCtx *m, String *content){
    String *s = String_From(m, bytes(okBase_cstr));
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddBytes(m, s, bytes("\r\n\r\n"), 4);
    String_Add(m, s, content);

    return s;
}

static char *errBase_cstr = "HTTP/1.1 500 Error\r\nServer: filestore\r\nContent-Length: ";
static String *packageError(MemCtx *m, String *content){
    String *s = String_From(m, bytes(errBase_cstr));
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddBytes(m, s, bytes("\r\n\r\n"), 4);
    String_Add(m, s, content);

    return s;
}

char *Method_ToString(int method){
    if(method == METHOD_GET){
        return "GET";
    }else if(method == METHOD_SET){
        return "SET";
    }else if(method == METHOD_UPDATE){
        return "UPDATE";
    }else{
        return "UNKONWN_method";
    }
}

status Req_Recv(Serve *sctx, Req *req){
    byte buff[SERV_READ_SIZE];
    size_t l = recv(req->fd, buff, SERV_READ_SIZE, 0);
    status r = NOOP;
    if(l > 0){
        String_AddBytes(req->m, req->in._shelf, buff, l);
        Debug_Print((void *)req->in._shelf, TYPE_STRING_CHAIN, "_shelf is", COLOR_CYAN, FALSE);
        r = Roebling_Run(req->in.sexp);
        if(r == ERROR){
            req->state = ERROR;
            Debug_Print((void *)req, TYPE_REQ, "Req Parsed (Error) ", COLOR_RED, TRUE);
        }else if (r == COMPLETE){
            req->state = PROCESSING;
            Debug_Print((void *)req, TYPE_REQ, "Req Parsed ", COLOR_CYAN, TRUE);
        }

        Serve_NextState(sctx, req);
        return req->state;
    }

    return NOOP;
}

status Req_Process(Serve *sctx, Req *req){
    req->out.response = packageResponse(req->m, String_From(req->m, bytes("poo")));
    req->out.cursor = SCursor_Make(req->m, req->out.response);

    req->state = RESPONDING;
    Serve_NextState(sctx, req);
    return req->state;
}

status Req_Handle(Serve *sctx, Req *req){
    if(req->state == INCOMING){
        return Req_Recv(sctx, req);
    }

    if(req->state == PROCESSING){
        return Req_Process(sctx, req);
    }

    return NOOP;
}

Req *Req_Make(Serve *sctx){
    MemCtx *m = MemCtx_Make();
    Req* req = (Req *)MemCtx_Alloc(m, sizeof(Req));
    req->sctx = sctx;
    req->direction = -1;
    req->in._shelf = String_Init(m, -1); 
    printf("_shelf type %s\n", Class_ToString(req->in._shelf->type.of));
    req->in.sexp = Roebling_Make(m, TYPE_STRUCTEXP, sctx->parsers, req->in._shelf, (void *)req);

    MemCtx_Bind(m, req);

    return req;
}

status Req_SetError(Serve *sctx, Req *req, String *msg){
    req->state = RESPONDING;
    Serve_NextState(sctx, req);
    req->out.response = packageError(req->m, msg);
    return SUCCESS;
}

