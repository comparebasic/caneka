#include <external.h>
#include <caneka.h>

static char *okBase_cstr = "HTTP/1.1 200 OK\r\nServer: caneka\r\nContent-Length: ";
static String *packageResponse(MemCtx *m, String *content){
    String *s = String_From(m, bytes(okBase_cstr));
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddBytes(m, s, bytes("\r\n\r\n"), 4);
    String_Add(m, s, content);

    return s;
}

static char *errBase_cstr = "HTTP/1.1 500 Error\r\nServer: caneka\r\nContent-Length: ";
static String *packageError(MemCtx *m, String *content){
    String *s = String_From(m, bytes(errBase_cstr));
    String *length_s = String_FromInt(m, (int)content->length); 
    String_Add(m, s, length_s);
    String_AddBytes(m, s, bytes("\r\n\r\n"), 4);
    String_Add(m, s, content);

    return s;
}

status Req_Recv(Serve *sctx, Req *req){
    byte buff[SERV_READ_SIZE];
    size_t l = recv(req->fd, buff, SERV_READ_SIZE, 0);
    status r = NOOP;
    if(l > 0){

        String_AddBytes(req->m, req->in.shelf, buff, l);
        Debug_Print((void *)req->in.shelf, TYPE_STRING_CHAIN, "shelf is", COLOR_CYAN, FALSE);

        r = Roebling_Run(req->in.rbl);
        if(r == ERROR){
            req->type.state = ERROR;
            Debug_Print((void *)req, TYPE_REQ, "Req Parsed (Error) ", COLOR_RED, TRUE);
        }else if (r == COMPLETE){
            req->type.state = PROCESSING;
            Debug_Print((void *)req, TYPE_REQ, "Req Parsed ", COLOR_CYAN, TRUE);
        }

        req->type.state |= NEXT;
        return req->type.state;
    }

    return NOOP;
}

Req *Req_Make(MemCtx *m, Serve *sctx, Proto *proto, int direction){
    MemCtx *rm = MemCtx_Make();
    Req* req = (Req *)MemCtx_Alloc(rm, sizeof(Req));
    req->type.of = TYPE_REQ;
    req->m = rm;
    req->sctx = sctx;
    req->proto = proto;
    req->direction = direction;
    req->in.shelf = String_Init(req->m, STRING_EXTEND);

    return req;
}
