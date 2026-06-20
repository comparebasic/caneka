#include <external.h>
#include <caneka.h>

status HttpReq_RespToRbl(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);

    struct pollfd *pfd = (struct pollfd *)&req->u;
    Buff_SetSocket(req->in, pfd->fd);
    if((req->rbl->type.state & (SUCCESS|ERROR)) == 0 &&
            (Buff_ReadAmount(req->in, SERVE_READ_SIZE) & NOOP) == 0){
        Roebling_Run(req->rbl);
    }
    
    req->type.state |= req->rbl->type.state & (SUCCESS|ERROR);

    if(req->type.state & SUCCESS){
        HttpReq_ParseBody(req);            
        if(req->type.state & DEBUG){
            void *args[] = {
                req,
                NULL,
            };
            Out("^0.Parsed Tcp Initial Request -> ^c.@^0\n", args);
        }
    }

    Return(m, req->type.state);
}

status HttpReq_ReadToRbl(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);

    struct pollfd *pfd = (struct pollfd *)&req->u;
    Buff_SetSocket(req->in, pfd->fd);
    if((req->rbl->type.state & (SUCCESS|ERROR)) == 0 &&
            (Buff_ReadAmount(req->in, SERVE_READ_SIZE) & NOOP) == 0){
        Roebling_Run(req->rbl);
    }
    
    req->type.state |= req->rbl->type.state & (SUCCESS|ERROR);

    if(req->type.state & SUCCESS){
        HttpReq_ParseBody(req);            
        if(req->type.state & DEBUG){
            void *args[] = {
                req,
                NULL,
            };
            Out("^0.Parsed Tcp Initial Request -> ^c.@^0\n", args);
        }
    }

    Return(m, req->type.state);
}

status HttpReq_Write(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);

    struct pollfd *pfd = (struct pollfd *)&req->u;
    Buff_SetSocket(req->out, pfd->fd);

    Iter it;
    Iter_Init(&it, req->sections);
    while((Iter_Next(&it) & END) == 0){
        Buff *bf = Iter_Get(&it);

        Buff_Pipe(req->out, bf);
        if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
            close(bf->fd);
            Buff_UnsetFd(bf);
        }

        if((bf->type.state & ERROR) || (bf->type.state & (SUCCESS|END)) == 0){
            req->type.state |= ERROR;
            break;
        }
    }

    if((req->type.state & ERROR) == 0){
        req->type.state |= SUCCESS;
    }
    
    Return(m, req->type.state);
}

void HttpReq_ParseBody(HttpReq *req){
    MemCh *m = req->m;
    Debug_Push(m, req);
    Abstract *value = Table_Get(req->headersIt.p, K(m, "Content-Length"));
    if(value != NULL){
        i64 length = (i64)((Single *)value)->val.value;
        Cursor *curs = req->rbl->curs;
        Cursor_Incr(curs, 1); /* TODO: remove */

        i32 remaining = length - (curs->v->total - curs->pos);
        while(remaining > 0 && (req->in->type.state & ERROR) == 0){
            Buff_ReadAmount(req->in, remaining);
            remaining = length - (curs->v->total - curs->pos);
        }

        Hashed *typeH = Table_Get(req->headersIt.p, K(m, "Content-Type"));
        if(typeH != NULL){
            /* parse stuff here such as json or form data */
        }
        Buff *bf = Buff_Make(m, ZERO);
        Cursor_Remaining(curs, bf);
        req->body = (void *)bf;
    }

    ReturnVoid(m);
}

void HttpReq_Close(HttpReq *req){
    if(req->in != NULL){
        close(req->in->fd);
        Buff_UnsetFd(req->in);
    }
    if(req->out != NULL){
        close(req->out->fd);
        Buff_UnsetFd(req->out);
    }
}

void HttpReq_SetFd(HttpReq *req, i32 fd){
    struct pollfd *pfd = (struct pollfd *)&req->u;
    pfd->fd = fd;
}

void HttpReq_ExpectRecv(HttpReq *req){
    struct pollfd *pfd = (struct pollfd *)&req->u;
    pfd->events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
}

void HttpReq_ExpectSend(HttpReq *req){
    struct pollfd *pfd = (struct pollfd *)&req->u;
    pfd->events = POLLOUT|POLLNVAL|POLLHUP|POLLERR;
}

Req *HttpReq_Mk(MemCh *m, Serve *srv){
    HttpReq *req = MemCh_AllocOf(m, sizeof(HttpReq), TYPE_HTTP_REQ);
    req->type.of = TYPE_HTTP_REQ;
    req->m = m;
    Iter_Init(&req->headersIt, Table_Make(m));
    Iter_Init(&req->queryIt, Table_Make(m));
    req->meta = Table_Make(m);
    req->in = Buff_Make(m, ZERO);
    req->out = Buff_Make(m, BUFF_UNBUFFERED);
    req->sections = Span_Make(m);
    return (Req *)req;
}

void HttpReq_Setup(MemCh *m, Req *_req, Serve *srv){
    HttpReq *req = (HttpReq *)_req;
    srv->metrics.open++;

    TcpSource *ts = (TcpSource *)srv->source;
    req->clientEnt = ts->clientEnt;
    HttpReq_SetFd(req, ts->new_fd);
    HttpReq_ExpectRecv(req);

    Queue_SetCriteria(srv->q, 0, req->idx, &req->u);
}

void HttpReq_SetToRecv(HttpReq *req){
    MemCh *m = req->m;
    req->rbl = HttpRbl_Make(m, Cursor_Make(m, req->in->v), req);
}

void HttpReq_SetToResponse(HttpReq *req, i32 fd){
    MemCh *m = req->m;
    if(fd >= 0){
        HttpReq_SetFd(req, fd);
    }
    req->rbl = HttpRespRbl_Make(m, Cursor_Make(m, req->in->v), req);
    req->type.state |= HTTP_REQ_RESPONSE;
}
