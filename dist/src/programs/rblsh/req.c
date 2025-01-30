#include <external.h>
#include <caneka.h>
#include <rblsh.h>

static status RblSh_RecvCmd(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_TermIo(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_Driver(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    if((r & NOOP) == 0){
        IoProto *def = (IoProto*)as(req->proto, TYPE_IO_PROTO);
        RblShCtx *ctx = (RblShCtx*)def->custom;
        Roebling_AddReset(ctx->rbl);
        status rr = Roebling_Run(ctx->rbl);
        return h->type.state;
    }
    return h->type.state;
}

static status RblSh_Status(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_Wait(Handler *h, Req *req, Serve *sctx){
    Handler *recv = Handler_Make(req->m, RblSh_RecvCmd, NULL, SOCK_IN);
    Handler_AddPrior(req->m, h, recv);
    return h->type.state;
}

Handler *RblSh_GetHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    Handler *h = NULL;
    if((req->type.state & INREQ) != 0){
        h = Handler_Make(m, RblSh_Wait, NULL, SOCK_IN);
    }else if((req->type.state & DRIVEREQ) != 0){
        h = Handler_Make(m, RblSh_Driver, NULL, SOCK_IN);
    }else{
        h = Handler_Make(m, RblSh_TermIo, NULL, SOCK_OUT);
    }
    return h;
}

Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx){
    DebugStack_Push("RblSh_ReqMake", TYPE_CSTR); 
    MemCtx *m = MemCtx_Make();
    SubProto *proto = (SubProto *)SubProto_Make(m, (ProtoDef *)sctx->def);
    if((sctx->type.state & DRIVEREQ) != 0){
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        req->type.state |= (sctx->type.state & (INREQ|OUTREQ|ERRREQ|DRIVEREQ));
        req->in.rbl = RblShParser_Make(m, NULL, req->in.shelf);
        DebugStack_Pop();
        return req;
    }else{
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        req->type.state |= (sctx->type.state & (INREQ|OUTREQ|ERRREQ|DRIVEREQ));
        if((sctx->type.of & INREQ) != 0){
            req->in.rbl = RblShParser_Make(m, 
                (RblShCtx *)sctx->def->source,
                req->in.shelf);
        }else{
            req->in.rbl = TermIo_RblMake(m, 
                req->in.shelf,
                (Abstract *)super,
                RblShSuper_Capture);
        }
        DebugStack_Pop();
        return req;
    }
}
