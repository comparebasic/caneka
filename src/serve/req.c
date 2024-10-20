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

status Req_Respond(Serve *sctx, Req *req){
    if(req->out.response == NULL){
        return ERROR;
    }else{
        size_t l = write(req->fd, req->out.response->bytes, req->out.response->length);
    }
    return SUCCESS;
}

status Req_Recv(Serve *sctx, Req *req){
    byte buff[SERV_READ_SIZE];
    ssize_t l = read(req->fd, buff, SERV_READ_SIZE);
    status r = NOOP;
    if(l > 0){
        Roebling_AddBytes(req->in.rbl, buff, l);
        return Roebling_Run(req->in.rbl);
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
