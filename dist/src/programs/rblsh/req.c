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
    MemCtx *m = MemCtx_Make();
    if((sctx->type.state & DRIVEREQ) != 0){
        IoProto *proto = (IoProto *)IoProto_Make(m, (ProtoDef *)sctx->def);
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        req->in.rbl = (Roebling *)RoeblingBlank_Make(m, 
            req->in.shelf, NULL, RblShCtx_Capture); 
        req->type.state |= (sctx->type.state & (INREQ|OUTREQ|ERRREQ|DRIVEREQ));
        return req;
    }else{
        SubProto *proto = (SubProto *)SubProto_Make(m, (ProtoDef *)sctx->def);
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        RblShSuper *super = RblShSuper_Make(m, req);
        proto->super = (Abstract *)super;

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
        req->type.state |= (sctx->type.state & (INREQ|OUTREQ|ERRREQ|DRIVEREQ));
        return req;
    }
}
