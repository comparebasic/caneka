#include <external.h>
#include <caneka.h>


void HttpReq_TlsReadToRbl(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;

    status r = READY;
    if((hreq->rbl->type.state & (SUCCESS|ERROR)) == 0){
        r = req->def->capsule->readTo(hreq->cap);
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
        Req_StepHandled(m, req, srv);
    }
}

void HttpReq_TlsFinalize(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    req->def->capsule->close(hreq->cap);
    HttpReq_Finalize(m, req, srv);
}

void HttpReq_TlsAccept(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    if(req->def->capsule == NULL){
        void*ar[] = {
            req,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Capsule required on HandlerDef for TLS envelope", ar);
        return;
    }

    if(hreq->cap == NULL){
        TlsInfo *info = TlsInfo_Make(m, req->crit->pfd.fd, req->def->ent->ctx);
        hreq->cap = Capsule_Make(m, TYPE_TLS_CAPSULE, hreq->in, hreq->out, info); 
    }

    if(req->def->capsule->open(hreq->cap) & SUCCESS){;
        Req_StepHandled(m, req, srv);
    }
}

void HttpReq_TlsWrite(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    hreq->out->type.state &= ~BUFF_UNBUFFERED;

    HttpReq_SetHeader(hreq, S(m, "Server"), S(m, "Caneka/1.0.0-alpha"));

    HttpReq_WriteStatus(m, hreq, req);
    HttpReq_SetLength(m, hreq);
    HttpReq_WriteHeaders(m, hreq);
    HttpReq_WriteBody(m, hreq);

    req->def->capsule->writeTo(hreq->cap);

    Req_StepHandled(m, req, srv);
}

void HttpTls_EntSetup(MemCh *m, HostEnt *ent, Node *config){
    Table *props = Span_Get(config, INST_PROPIDX_CHILDREN);
    ent->ctx = TlsCtx_Make(m,
        Table_Get(props, K(m, "tls-cert")), Table_Get(props, K(m, "tls-key")));
}

HandlerDef *HttpTlsReq_DefMake(MemCh *m,
        Span *steps, Node *ext, HostEnt *ent, Node *config){

    HttpTls_EntSetup(m, ent, config);
    HandlerDef *def = HandlerDef_Make(m);

    def->ent = ent;
    def->extra = (SourceMakerFunc)HttpReq_SourceMake;
    def->finalize = (ReqFunc) HttpReq_TlsFinalize;
    def->setup = (DoFunc) HttpReq_Setup;
    def->log.open = (ReqFunc) HttpReq_LogOpen;
    def->log.final = (ReqFunc) HttpReq_LogFinalized;
    def->capsule = Lookup_Get(CapsuleDefLookup, TYPE_TLS_CAPSULE);

    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsWrite, SEND_FLAGS));
    Span_AddSpanRev(def->route, steps);
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsReadToRbl, RECV_FLAGS));
    Span_Add(def->route, Func_Wrapped(m, HttpReq_TlsAccept, BIDIR_FLAGS));

    def->ext = ext;
    return def;
}
