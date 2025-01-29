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
    debug_Init(m);

    RblShCtx *ctx = RblShCtx_Make(m);

    Handler *h = NULL;
    ProtoDef *def = IoProtoDef_Make(m, (Maker)RblSh_ReqMake);
    def->getHandler = RblSh_getHandler;
    def->source = (Abstract *)ctx;
    Serve *sctx = Serve_Make(m, def);

    Req *req = Serve_AddFd(sctx, 0);
    ctx->shelf = req->in.shelf;

    SetOriginalTios();
    RawMode(TRUE);

    Serve_Run(sctx);

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
