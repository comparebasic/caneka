#include <external.h>
#include <caneka.h>

static status RblSh_RecvCmd(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_TermIo(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_Status(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_RunCmd(Handler *h, Req *req, Serve *sctx){
    SubProcess(req->m, req->data, NULL, (Abstract *)sctx);
    return h->type.state;
}

static status RblSh_Wait(Handler *h, Req *req, Serve *sctx){
    Handler *recv = Handler_Make(req->m, RblSh_RecvCmd, NULL, SOCK_IN);
    Handler_AddPrior(req->m, h, recv);
    return h->type.state;
}

static Handler *RblSh_getHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    if(req->type.state & INREQ){
        Handler *startHandler = Handler_Make(m, RblSh_Wait, NULL, SOCK_IN);
    }else{
        Handler *termioHandler = Handler_Make(m, RblSh_TermIo, NULL, SOCK_IN);
    }
    return startHandler;
}

Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    IoProto *proto = (IoProto *)IoProto_Make(m, (ProtoDef *)sctx->def);

    Req *req =  Req_Make(m, sctx, (Proto *)proto);
    RblShSuper_Make(req->m, req);
    if((sctx->type.of & INREQ) != 0){
        req->in.rbl = RblShParser_Make(m, 
            (RblShCtx *)sctx->def->source,
            req->in.shelf);
    }else{
        req->in.rbl = RblTermIo_Make(m, 
            (RblShCtx *)sctx->def->source,
            req->in.shelf);
    }
    req->type.state |= sctx->type.of & (INREQ|OUTREQ|ERRREQ);
    req->data = Span_Make(m, TYPE_SPAN);

    return req;
}
