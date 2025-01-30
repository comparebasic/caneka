#include <external.h>
#include <caneka.h>
#include <rblsh.h>

void Cleanup(Abstract *a){
    RawMode(FALSE);
}

int main(int argc, char **argv){
    status r = READY;

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    RblShDebug_Init(m);

    DebugStack_Push("rblsh", TYPE_CSTR); 

    RblShCtx *ctx = RblShCtx_Make(m);

    Handler *h = NULL;

    ProtoDef *def = IoProtoDef_Make(m, (Maker)RblSh_ReqMake);
    def->getHandler = RblSh_GetHandler;
    def->source = (Abstract *)ctx;
    Serve *sctx = Serve_Make(m, def);

    sctx->type.state |= DRIVEREQ;
    Req *req = Serve_AddFd(sctx, 0);
    sctx->type.state &= ~DRIVEREQ;

    req->in.rbl->source = (Abstract *)ctx;
    ((IoProto *)as(req->proto, TYPE_IO_PROTO))->custom = (Abstract *)ctx;

    Roebling_Reset(m, ctx->rbl, req->in.rbl->cursor.s);

    SetOriginalTios();
    RawMode(TRUE);

    ctx->sctx = sctx;
    Serve_RunFds(sctx);

    RawMode(FALSE);

    DebugStack_Pop();

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
