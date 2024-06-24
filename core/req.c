#include <external.h>
#include <filestore.h>

static char *okBase_cstr = "HTTP/1.1 200 OK\r\nServer: filestore\r\nContent-Length: ";
static String *packageResponse(MemCtx *m, String *content){
    String *s = String_From(m, okBase_cstr);
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddCstr(m, s, "\r\n\r\n", 4);
    String_Add(m, s, content);

    return s;
}

static char *errBase_cstr = "HTTP/1.1 500 Error\r\nServer: filestore\r\nContent-Length: ";
static String *packageError(MemCtx *m, String *content){
    String *s = String_From(m, errBase_cstr);
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddCstr(m, s, "\r\n\r\n", 4);
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
        String_AddCStr(req->m, req->_shelf, buff, l);
        r = StructExp_Run(req->sexp);
        if(r == ERROR){
            req->state = ERROR;
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
    req->response = packageResponse(req->m, String_From(req->m, "poo"));
    req->cursor = SCursor_Make(req->m, req->response);

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
    req->direction = -1;
    req->_shelf = String_Init(m); 
    req->sexp = StructExp_Make(m, TYPE_STRUCTEXP, sctx->parsers, req->_shelf, (void *)req);

    MemCtx_Bind(m, req);

    return req;
}

status Req_SetError(Serve *sctx, Req *req, String *msg){
    req->state = RESPONDING;
    Serve_NextState(sctx, req);
    req->response = packageError(req->m, msg);
    return SUCCESS;
}

