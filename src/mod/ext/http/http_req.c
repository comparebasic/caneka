#include <external.h>
#include <caneka.h>

static void HttpReq_setLength(MemCh *m, HttpReq *req){
    i64 length = 0;
    Iter it;
    if(req->sections->nvalues > 0){
        Iter_Init(&it, req->sections);
        while((Iter_Next(&it) & END) == 0){
            Buff *bf = Iter_Get(&it);
            Buff_Stat(bf);
            length += bf->st.st_size;
        }
    }
    if(length > 0){
        HttpReq_SetHeader(req,
            S(m, "Content-Length"), I32_Wrapped(m, length));
    }
}

static void HttpReq_writeStatus(MemCh *m, HttpReq *req){
    if(req->type.state & NOOP){
        if(req->type.state & ERROR){
            Buff_Add(req->out, K(m, "HTTP/1.1 404 Not Found\r\n"));
        }else{
            Buff_Add(req->out, K(m, "HTTP/1.1 304 Not Modified\r\n"));
        }
    }else if(req->type.state & ERROR){
        Buff_Add(req->out, K(m, "HTTP/1.1 500 Server Error\r\n"));
    }else{
        Buff_Add(req->out, K(m, "HTTP/1.1 200 Ok\r\n"));
    }
}

static void HttpReq_writeHeaders(MemCh *m, HttpReq *req){
    Iter it;
    Iter_Init(&it, Table_Ordered(m, req->headersOut));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        void *ar[] = {h->key, h->value, NULL};
        Fmt(req->out, "$: $\r\n", ar);
    }
    Buff_Add(req->out, K(m, "\r\n"));
}

static void HttpReq_writeBody(MemCh *m, HttpReq *req){
    Iter it;
    if(req->sections->nvalues > 0){
        Iter_Init(&it, req->sections);
        while((Iter_Next(&it) & END) == 0){
            Buff *bf = Iter_Get(&it);
            Buff_Pipe(req->out, bf);
            if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
                close(bf->fd);
                Buff_UnsetFd(bf);
            }
        }
    }
}

status HttpReq_RespToRbl(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);

    struct pollfd *pfd = (struct pollfd *)req->slot;
    Buff_SetSocket(req->in, pfd->fd);
    if((req->rbl->type.state & (SUCCESS|ERROR)) == 0 &&
            (Buff_ReadAmount(req->in, SERVE_READ_SIZE) & NOOP) == 0){
        Roebling_Run(req->rbl);
        if(req->type.state & DEBUG){
            void *ar[] = {
                req->in->v,
                req->headersIt.p,
                NULL
            };
            Out("^p.  Read So Far: ^c.@ -> @^0\n", ar);
        }
    }
    
    req->type.state |= req->rbl->type.state & (SUCCESS|ERROR);

    if(req->type.state & SUCCESS){
        HttpReq_ParseBody(req);            
        if(req->type.state & DEBUG){
            void *args[] = {
                req,
                NULL,
            };
            Out("^p.  Parsed Tcp Initial Request -> ^c.@^0\n", args);
        }
    }else{
        if(req->type.state & DEBUG){
            void *args[] = {
                req,
                NULL,
            };
            Out("^r.  Error Parsing Tcp Initial Request -> ^c.@^0\n", args);
        }
    }

    HttpReq_ExpectInternal(req);

    Return(m, req->type.state);
}

void HttpReq_SetHeader(HttpReq *req, Str *key, void *value){
    if(req->headersOut == NULL){
        req->headersOut = Table_Make(req->m);
    }
    Table_Set(req->headersOut, key, value);
}

void HttpReq_RemoveHeader(HttpReq *req, Str *key){
    if(req->headersOut != NULL){
        req->headersOut = Table_Make(req->m);
        Table_UnSet(req->headersOut, key);
    }
}

status HttpReq_ReadToRbl(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);

    struct pollfd *pfd = (struct pollfd *)req->slot;
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

    HttpReq_SetHeader(req, S(m, "Server"), S(m, "Caneka/1.0.0-alpha"));
    struct pollfd *pfd = (struct pollfd *)req->slot;
    Buff_SetSocket(req->out, pfd->fd);

    printf("Write\n");
    fflush(stdout);

    HttpReq_writeStatus(m, req);
    HttpReq_setLength(m, req);
    HttpReq_writeHeaders(m, req);
    HttpReq_writeBody(m, req);

    printf("Written\n");
    fflush(stdout);
    req->type.state |= (SUCCESS|END);
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
    struct pollfd *pfd = (struct pollfd *)req->slot;
    pfd->fd = fd;

    printf("Setting fd%d\n", pfd->fd);
    fflush(stdout);
}

void HttpReq_ExpectRecv(HttpReq *req){
    struct pollfd *pfd = (struct pollfd *)req->slot;
    pfd->events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
}

void HttpReq_ExpectInternal(HttpReq *req){
    struct pollfd *pfd = (struct pollfd *)req->slot;
    pfd->events = POLLNVAL|POLLHUP|POLLERR;
}

void HttpReq_ExpectSend(HttpReq *req){
    struct pollfd *pfd = (struct pollfd *)req->slot;
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
    req->crit = ReqCrit_Make(rm);
    req->crit->pfd->fd = ts->new_fd;

    HttpReq_SetFd(req, ts->new_fd);
    HttpReq_ExpectRecv(req);
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
