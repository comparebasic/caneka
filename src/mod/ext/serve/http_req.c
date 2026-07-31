#include <external.h>
#include <caneka.h>

void HttpReq_LogOpen(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

void HttpReq_LogFinalized(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

void HttpReq_SetLength(MemCh *m, HttpReq *hreq){
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

void HttpReq_WriteStatus(MemCh *m, HttpReq *hreq, Req *req){
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

void HttpReq_WriteHeaders(MemCh *m, HttpReq *hreq){
    Iter it;
    Iter_Init(&it, Table_Ordered(m, hreq->headersOut));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        void *ar[] = {h->key, h->value, NULL};
        Fmt(hreq->out, "$: $\r\n", ar);
    }
    Buff_Add(hreq->out, K(m, "\r\n"));
}

void HttpReq_WriteBody(MemCh *m, HttpReq *hreq){
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

void HttpReq_Close(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
}

void HttpReq_Finalize(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    close(req->crit->pfd.fd);

    Buff *bf = NULL;
    if(req->type.state & ERROR && srv->log.err != NULL){
        bf = srv->log.err;
    }else{
        bf = srv->log.out;
    }

    Table *tbl = Table_Make(m);

    Single *st = NULL;
    Str *format = NULL;

    if((req->type.state & (ERROR|NOOP)) == (ERROR|NOOP)){
        st = I32_Wrapped(m, 404);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiYellow;
        }
    }else if(req->type.state & ERROR){
        st = I32_Wrapped(m, 500);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiRed;
        }
    }else if(req->type.state & NOOP){
        st = I32_Wrapped(m, 304);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiCyan;
        }
    }else{
        st = I32_Wrapped(m, 200);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiCyan;
        }
    }

    Time_Now(&req->metrics.end);
    duration d = Time_Duration(m, &req->metrics.end, &req->metrics.start);
    Table_Set(tbl, K(m, "path"), hreq->path);
    Table_Set(tbl, K(m, "status"), st);
    Table_Set(tbl, K(m, "duration"), Duration_Str(m, d));
    Table_Set(tbl, K(m, "from"), hreq->addr);

    Log_Flat(m, bf, K(m, "Request "), Table_Ordered(m, tbl), format);
}

void HttpReq_RespToRbl(MemCh *m, Req *req, Serve *srv){
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

    ReturnVoid(m);
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

void HttpReq_ReadToRbl(MemCh *m, Req *req, Serve *srv){
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

    ReturnVoid(m);
}

void HttpReq_Write(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    Buff_SetSocket(hreq->out, req->crit->pfd.fd);
    HttpReq_SetHeader(hreq, S(m, "Server"), S(m, "Caneka/1.0.0-alpha"));
    HttpReq_WriteStatus(m, hreq, req);
    HttpReq_SetLength(m, hreq);
    HttpReq_WriteHeaders(m, hreq);
    HttpReq_WriteBody(m, hreq);

    req->type.state |= (SUCCESS|END);
    ReturnVoid(m);
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

HandlerDef *HttpReq_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->extra = (SourceMakerFunc)HttpReq_SourceMake;
    def->finalize = (ReqFunc) HttpReq_Finalize;
    def->log.open = (ReqFunc) HttpReq_LogOpen;
    def->log.final = (ReqFunc) HttpReq_LogFinalized;
    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_RespToRbl));
    Span_Add(def->route, Func_Wrapped(m, Req_Prepare));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_Write));

    return def;
}

void HttpReq_Init(MemCh *m){
    Lookup_Add(m, ErrorHandlers, TYPE_HTTP_REQ, (void *)HttpStatic_Error);
}
