#include <external.h>
#include <caneka.h>

static void HttpReq_setLength(MemCh *m, HttpReq *hreq){
    i64 length = 0;
    Iter it;
    if(hreq->sections->nvalues > 0){
        Iter_Init(&it, hreq->sections);
        while((Iter_Next(&it) & END) == 0){
            Buff *bf = Iter_Get(&it);
            Buff_Stat(bf);
            length += bf->st.st_size;
        }
    }
    if(length > 0){
        HttpReq_SetHeader(hreq,
            S(m, "Content-Length"), I32_Wrapped(m, length));
    }
}

static void HttpReq_writeStatus(MemCh *m, HttpReq *hreq, Req *req){
    char *st = NULL;
    if(req->type.state & NOOP){
        if(req->type.state & ERROR){
            st = "HTTP/1.1 404 Not Found\r\n";
        }else{
            st = "HTTP/1.1 304 Not Modified\r\n";
        }
    }else if(req->type.state & ERROR){
        st = "HTTP/1.1 500 Server Error\r\n";
    }else{
        st = "HTTP/1.1 200 Ok\r\n";
    }

    Str *s = K(m, st);
    Buff_Add(hreq->out, s);
}

static void HttpReq_writeHeaders(MemCh *m, HttpReq *hreq){
    Iter it;
    Iter_Init(&it, Table_Ordered(m, hreq->headersOut));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        void *ar[] = {h->key, h->value, NULL};
        Fmt(hreq->out, "$: $\r\n", ar);
    }
    Buff_Add(hreq->out, K(m, "\r\n"));
}

static void HttpReq_writeBody(MemCh *m, HttpReq *hreq){
    Iter it;
    if(hreq->sections->nvalues > 0){
        Iter_Init(&it, hreq->sections);
        while((Iter_Next(&it) & END) == 0){
            Buff *bf = Iter_Get(&it);
            Buff_Pipe(hreq->out, bf);
            if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
                close(bf->pfd->fd);
                Buff_UnsetFd(bf);
            }
        }
    }
}

status HttpReq_RespToRbl(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    if((hreq->rbl->type.state & (SUCCESS|ERROR)) == 0){
        if(req->def->capsule != NULL){
            req->def->capsule->readTo(hreq->cap);
        }else{
            Buff_ReadAmount(hreq->in, SERVE_READ_SIZE);
        }

        if((hreq->in->type.state & NOOP) == 0){
            Roebling_Run(hreq->rbl);
            if(req->type.state & DEBUG){
                void *ar[] = {
                    hreq->in->v,
                    hreq->headersIt.p,
                    NULL
                };
                Out("^p.  Read So Far: ^c.@ -> @^0\n", ar);
            }
        }
    }
    
    req->type.state |= hreq->rbl->type.state & (SUCCESS|ERROR);

    if(hreq->type.state & SUCCESS){
        HttpReq_ParseBody(hreq);            
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

    Req_ExpectInternal(req);

    Return(m, req->type.state);
}

void HttpReq_SetHeader(HttpReq *hreq, Str *key, void *value){
    if(hreq->headersOut == NULL){
        hreq->headersOut = Table_Make(hreq->m);
    }
    Table_Set(hreq->headersOut, key, value);
}

void HttpReq_RemoveHeader(HttpReq *req, Str *key){
    if(req->headersOut != NULL){
        req->headersOut = Table_Make(req->m);
        Table_UnSet(req->headersOut, key);
    }
}

status HttpReq_ReadToRbl(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    Buff_SetSocket(hreq->in, req->crit->pfd.fd);
    if((hreq->rbl->type.state & (SUCCESS|ERROR)) == 0 &&
            (Buff_ReadAmount(hreq->in, SERVE_READ_SIZE) & NOOP) == 0){
        Roebling_Run(hreq->rbl);
    }
    
    req->type.state |= hreq->rbl->type.state & (SUCCESS|ERROR);

    if(req->type.state & SUCCESS){
        HttpReq_ParseBody(hreq);            
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

status HttpReq_Write(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    HttpReq_SetHeader(hreq, S(m, "Server"), S(m, "Caneka/1.0.0-alpha"));
    if(hreq->cap == NULL){
        Buff_SetSocket(hreq->out, req->crit->pfd.fd);
    }

    HttpReq_writeStatus(m, hreq, req);
    HttpReq_setLength(m, hreq);
    HttpReq_writeHeaders(m, hreq);
    HttpReq_writeBody(m, hreq);

    if(hreq->cap != NULL){
        req->def->capsule->writeTo(hreq->cap);
    }

    req->type.state |= (SUCCESS|END);
    Return(m, req->type.state);
}

void HttpReq_ParseBody(HttpReq *hreq){
    MemCh *m = hreq->m;
    Debug_Push(m, hreq);
    Abstract *value = Table_Get(hreq->headersIt.p, K(m, "Content-Length"));
    if(value != NULL){

        i64 length = (i64)((Single *)value)->val.value;
        Cursor *curs = hreq->rbl->curs;
        Cursor_Incr(curs, 1); /* TODO: remove */

        i32 remaining = length - (curs->v->total - curs->pos);
        while(remaining > 0 && (hreq->in->type.state & ERROR) == 0){
            Buff_ReadAmount(hreq->in, remaining);
            remaining = length - (curs->v->total - curs->pos);
        }

        Hashed *typeH = Table_Get(hreq->headersIt.p, K(m, "Content-Type"));
        if(typeH != NULL){
            /* parse stuff here such as json or form data */
        }
        Buff *bf = Buff_Make(m, ZERO);
        Cursor_Remaining(curs, bf);
        hreq->body = (void *)bf;
    }

    ReturnVoid(m);
}


status HttpReq_Accept(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    if(req->def->capsule != NULL){
#ifdef CNKOPT_CRYPTO
        if(hreq->cap == NULL){
            TlsInfo *info = TlsInfo_Make(m, req->crit->pfd.fd, req->conn.ent->ctx);
            hreq->cap = Capsule_Make(m, TYPE_TLS_CAPSULE, hreq->in, hreq->out, info); 
        }
#endif
        if(req->def->capsule->open(hreq->cap) & SUCCESS){;
            req->type.state |= SUCCESS;
        }
    }else{
        Buff_SetSocket(hreq->in, req->crit->pfd.fd);
    }

    Return(m, req->type.state);
}

status HttpReq_Close(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    if(req->def->capsule != NULL){
        req->def->capsule->close(hreq->cap);
    }
    close(req->crit->pfd.fd);
    Return(m, req->type.state);
}

void HttpReq_Setup(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    srv->metrics.open++;

    TcpSource *ts = (TcpSource *)srv->source;
    hreq->addr = ts->addr;
    req->crit = ReqCrit_Make(m);
    req->crit->pfd.fd = ts->new_fd;
    if(req->def->capsule != NULL){
        hreq->out->type.state &= ~BUFF_UNBUFFERED;
    }

    Req_SetFd(req, ts->new_fd);
    Req_ExpectRecv(req);
}

void HttpReq_SetToRecv(HttpReq *hreq, Req *req){
    MemCh *m = hreq->m;
    hreq->rbl = HttpRbl_Make(m, Cursor_Make(m, hreq->in->v), hreq);
}

void HttpReq_SetToResponse(HttpReq *hreq, Req *req, i32 fd){
    MemCh *m = hreq->m;
    if(fd >= 0){
        Req_SetFd(req, fd);
    }
    hreq->rbl = HttpRespRbl_Make(m, Cursor_Make(m, hreq->in->v), hreq);
    req->type.state |= HTTP_REQ_RESPONSE;
}

void *HttpReq_SourceMake(MemCh *m, Serve *srv, HandlerDef *def){
    HttpReq *hreq = MemCh_AllocOf(m, sizeof(HttpReq), TYPE_HTTP_REQ);
    hreq->type.of = TYPE_HTTP_REQ;
    hreq->m = m;
    Iter_Init(&hreq->headersIt, Table_Make(m));
    Iter_Init(&hreq->queryIt, Table_Make(m));
    hreq->meta = Table_Make(m);
    hreq->in = Buff_Make(m, ZERO);
    hreq->out = Buff_Make(m, BUFF_UNBUFFERED);
    hreq->sections = Span_Make(m);
    return hreq;
}

status HttpStatic_Error(MemCh *m, HttpReq *req, ErrorMsg *msg){
    req->type.state |= ERROR;
    req->type.state &= ~NOOP;
    HttpReq_RemoveHeader(req, S(m, "Etag"));
    HttpReq_RemoveHeader(req, S(m, "Last-Modified"));

    Span_Wipe(req->sections);

    Buff *bf = Buff_Make(m, ZERO);
    Buff_Add(bf, S(m, "<h1>Error</h1><p>"));
    ErrorMsg_Fmt(bf, msg);
    Buff_Add(bf, S(m, "</p>"));

    Span_Add(req->sections, bf);
    return ZERO;
}

void HttpStatic_SetCmdFile(MemCh *m, Serve *srv, Buff *file){
    MemCh *rm = MemCh_Make();

    HttpReq *req = rm->owner = (HttpReq *)srv->def->mk(rm, srv);
    req->key = S(m, "cmd");
    Time_Now(&req->metrics.start);

    req->crit = ReqCrit_Make(m);
    req->crit->pfd.fd = file->fd;
    req->idx = Queue_Add(srv->q, req, req->crit);
    HttpReq_ExpectRecv(req);

}

HandlerDef *HttpReq_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->mk = HttpReq_Mk;
    def->setup = HttpStatic_Setup;
    def->handle = (ReqFunc) HttpStatic_Handle;
    def->finalize = (ReqFunc) HttpStatic_Finalize;
    def->log.open = (ReqFunc) HttpStatic_logOpen;
    def->log.final = (ReqFunc) HttpStatic_logFinalized;
    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_RespToRbl));
    Span_Add(def->route, Func_Wrapped(m, HttpStatic_RetrieveFile));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Write));

    return def;
}

HandlerDef *HttpTlsReq_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->mk = HttpReq_Mk;
    def->setup = HttpStatic_Setup;
    def->handle = (ReqFunc) HttpStatic_Handle;
    def->finalize = (ReqFunc) HttpStatic_Finalize;
    def->log.open = (ReqFunc) HttpStatic_logOpen;
    def->log.final = (ReqFunc) HttpStatic_logFinalized;
    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Accept));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_RespToRbl));
    Span_Add(def->route, Func_Wrapped(m, HttpStatic_RetrieveFile));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Write));

    return def;
}

void HttpReq_Init(MemCh *m){
    Lookup_Add(m, ErrorHandlers, TYPE_HTTP_REQ, (void *)HttpStatic_Error);
}
