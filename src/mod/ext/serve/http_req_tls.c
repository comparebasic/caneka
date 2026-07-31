#include <external.h>
#include <caneka.h>

void HttpReq_TlsReadToRbl(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    if((hreq->rbl->type.state & (SUCCESS|ERROR)) == 0){
        req->def->capsule->readTo(hreq->cap);
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

void HttpReq_TlsFinalize(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    req->def->capsule->close(hreq->cap);
    HttpReq_Finalize(m, req, srv);
}

void HttpReq_TlsAccept(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    if(req->def->capsule != NULL){
        if(hreq->cap == NULL){
            TlsInfo *info = TlsInfo_Make(m, req->crit->pfd.fd, req->conn.ent->ctx);
            hreq->cap = Capsule_Make(m, TYPE_TLS_CAPSULE, hreq->in, hreq->out, info); 
        }
        if(req->def->capsule->open(hreq->cap) & SUCCESS){;
            req->type.state |= SUCCESS;
        }
    }else{
        Buff_SetSocket(hreq->in, req->crit->pfd.fd);
    }

    ReturnVoid(m);
}

void HttpReq_TlsWrite(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    HttpReq_SetHeader(hreq, S(m, "Server"), S(m, "Caneka/1.0.0-alpha"));

    HttpReq_WriteStatus(m, hreq, req);
    HttpReq_SetLength(m, hreq);
    HttpReq_WriteHeaders(m, hreq);
    HttpReq_WriteBody(m, hreq);

    req->def->capsule->writeTo(hreq->cap);

    req->type.state |= (SUCCESS|END);
    ReturnVoid(m);
}


HandlerDef *HttpTlsReq_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->extra = (SourceMakerFunc)HttpReq_SourceMake;
    def->finalize = (ReqFunc) HttpReq_TlsFinalize;
    def->log.open = (ReqFunc) HttpReq_LogOpen;
    def->log.final = (ReqFunc) HttpReq_LogFinalized;
    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsAccept));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsReadToRbl));
    Span_Add(def->route, Func_Wrapped(m, Req_Prepare));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsWrite));

    return def;
}
