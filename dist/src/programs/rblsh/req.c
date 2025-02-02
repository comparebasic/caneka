#include <external.h>
#include <caneka.h>
#include <rblsh.h>

static status RblSh_RecvCmd(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}

static status RblSh_TermIo(Handler *h, Req *req, Serve *sctx){
    SubProto *proto = (SubProto *)req->proto;
    RblShCtx *ctx = (RblShCtx *)as(sctx->def->source, TYPE_RBLSH_CTX);
    status r = Req_Read(sctx, req);
    if((r & PROCESSING) != 0){
        Cursor_Flush(ctx->m, req->out.cursor, ctx->out, (Abstract *)ctx); 
        ProcIoSet_SegFlags(proto->procio, sctx, PROCESSING);
    }

    if(proto->procio->pd.code != -1 && (r & PROCESSING) == 0){
        h->type.state |= SUCCESS;
        ProcIoSet_SegFlags(proto->procio, sctx, END);
    }

    h->type.state |= (SubStatus(&proto->procio->pd) & ERROR);

    return h->type.state;
}

static status RblSh_Driver(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    if((r & NOOP) == 0){
        SubProto *def = (SubProto*)as(req->proto, TYPE_SUB_PROTO);
        RblShCtx *ctx = (RblShCtx*)as(def->custom, TYPE_RBLSH_CTX);
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
    if((req->type.state & PROCIO_INREQ) != 0){
        h = Handler_Make(m, RblSh_Wait, NULL, SOCK_IN);
    }else if((req->type.state & DRIVEREQ) != 0){
        h = Handler_Make(m, RblSh_Driver, NULL, SOCK_IN);
    }else{
        h = Handler_Make(m, RblSh_TermIo, NULL, SOCK_OUT);
    }
    return h;
}

Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx, word flags){
    DebugStack_Push("RblSh_ReqMake", TYPE_CSTR); 
    MemCtx *m = MemCtx_Make();
    SubProto *proto = (SubProto *)SubProto_Make(m, (ProtoDef *)sctx->def, flags);
    if((flags & DRIVEREQ) != 0){
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        req->type.state |= flags;
        req->in.rbl = RblShParser_Make(m, req->in.shelf, (Abstract *)req);
        DebugStack_Pop();
        return req;
    }else{
        Req *req =  Req_Make(m, sctx, (Proto *)proto);
        req->type.state |= flags;
        if((flags & PROCIO_INREQ) != 0){
            req->in.rbl = RblShParser_Make(m, 
                req->in.shelf,(Abstract *)req);
        }else{
            req->in.rbl = TermIo_RblMake(m, 
                req->in.shelf, RblShSuper_Capture, (Abstract *)req);
            req->out.response = req->in.shelf;
            req->out.cursor = Cursor_Make(m, req->out.response);
        }
        DebugStack_Pop();
        return req;
    }
}
