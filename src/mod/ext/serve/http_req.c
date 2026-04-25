#include <external.h>
#include <caneka.h>

void HttpReq_ReadToRbl(Req *req){
    MemCh *m = req->m;
    Debug_Push(m, req);

    HttpReq *http = (HttpReq*)Ifc(m, req, TYPE_HTTP_REQ);

    struct pollfd *pfd = (struct pollfd *)&http->u;
    Buff_SetSocket(http->in, pfd->fd);
    if((Buff_ReadAmount(http->in, SERVE_READ_SIZE) & NOOP) == 0){
        Roebling_Run(http->rbl);
    }
    
    http->type.state |= http->rbl->type.state & (SUCCESS|ERROR);
    if((http->type.state & SUCCESS) && (http->type.state & DEBUG)){
        void *args[] = {
            http,
            NULL,
        };
        Out("^0.Parsed Tcp Initial Request -> ^c.@^0\n", args);
    }

    Debug_Pop(m);
}

void HttpReq_Write(Req *req){
    MemCh *m = req->m;
    Debug_Push(m, req);

    HttpReq *http = (HttpReq*)Ifc(m, req, TYPE_HTTP_REQ);

    struct pollfd *pfd = (struct pollfd *)&http->u;
    Buff_SetSocket(http->out, pfd->fd);

    Iter it;
    Iter_Init(&it, http->sections);
    while((Iter_Next(&it) & END) == 0){
        Buff *bf = Iter_Get(&it);

        Buff_Pipe(http->out, bf);
        if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
            close(bf->fd);
            Buff_UnsetFd(bf);
        }

        if((bf->type.state & ERROR) || (bf->type.state & (SUCCESS|END)) == 0){
            http->type.state |= ERROR;
            break;
        }
    }

    if((http->type.state & ERROR) == 0){
        http->type.state |= SUCCESS;
    }
    
    Debug_Pop(m);
}

void HttpReq_ExpectRecv(Req *req){
    struct pollfd *pfd = (struct pollfd *)&req->u;
    pfd->events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
}

void HttpReq_ExpectSend(Req *req){
    struct pollfd *pfd = (struct pollfd *)&req->u;
    pfd->events = POLLOUT|POLLNVAL|POLLHUP|POLLERR;
}

Req *HttpReq_Mk(IoCtx *ctx){
    MemCh *m = MemCh_Make();
    HttpReq *req = MemCh_AllocOf(m, sizeof(HttpReq), TYPE_HTTP_REQ);
    req->type.of = TYPE_HTTP_REQ;
    req->m = m;
    req->headers = Table_Make(m);
    req->meta = Table_Make(m);
    req->in = Buff_Make(m, ZERO);
    req->out = Buff_Make(m, BUFF_UNBUFFERED);
    req->sections = Span_Make(m);
    req->rbl = HttpRbl_Make(m, Cursor_Make(m, req->in->v), req);
    return (Req *)req;
}
