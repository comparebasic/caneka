#include <external.h>
#include <caneka.h>

static status RblSh_RecvCmd(Handler *h, Req *req, Serve *sctx){
    h->type.state |= SUCCESS;

    status r = Req_Recv(sctx, req);
    h->type.state |= (r & (SUCCESS|ERROR));

    return h->type.state;
}

static status RblSh_RunCmd(Handler *h, Req *req, Serve *sctx){
    h->type.state |= SUCCESS;
    return h->type.state;
}


static status RblSh_Wait(Handler *h, Req *req, Serve *sctx){
    h->type.state |= SUCCESS;
    Span_Add(h->prior, (Abstract *)Handler_Make(req->m, RblSh_RecvCmd, NULL, SOCK_IN));
    return h->type.state;
}

static Handler *RblSh_getHandler(Serve *sctx, Req *req){
    MemCtx *m = req->m;
    Handler *startHandler = Handler_Make(m, RblSh_Wait, NULL, SOCK_IN);
    return startHandler;
}

static status RblSh_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Debug_Print((void *)s, 0, "Captured: ", COLOR_YELLOW, FALSE);
    printf("\n");
    return SUCCESS;
}

static Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx){
    Stack(bytes("RblSh_ReqMake"), NULL);
    MemCtx *m = MemCtx_Make();
    IoProto *proto = (IoProto *)IoProto_Make(m, (ProtoDef *)sctx->def);

    Req *req =  Req_Make(m, sctx, (Proto *)proto);
    req->in.rbl = RblShParser_Make(m, 
        (RblShCtx *)sctx->def->source,
        req->in.shelf, 
        RblSh_Capture);

    Return req;
}

static RblShCtx *RblShCtx_Make(MemCtx *m){
    RblShCtx *ctx = MemCtx_Alloc(m, sizeof(RblShCtx));
    ctx->type.of = TYPE_RBLSH_CTX;

    char buff[PATH_BUFFLEN];
    char *path = getcwd(buff, PATH_BUFFLEN);
    ctx->cwd.s = String_Make(m, bytes(path));
    ctx->cwd.p = String_SplitToSpan(m, ctx->cwd.s, String_Make(m, bytes("/")));

    Debug_Print((void *)ctx->cwd.s, 0, "Cwd(s):", COLOR_PURPLE, FALSE);
    printf("\n");
    Debug_Print((void *)ctx->cwd.p, 0, "Cwd(p):", COLOR_PURPLE, TRUE);
    printf("\n");

    return ctx;
}

int main(int argc, char **argv){
    status r = READY;

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);

    RblShCtx *ctx = RblShCtx_Make(m);

    Handler *h = NULL;
    Req *req = NULL;;
    ProtoDef *def = IoProtoDef_Make(m, (Maker)RblSh_ReqMake);
    def->getHandler = RblSh_getHandler;
    def->source = (Abstract *)ctx;
    Serve *sctx = Serve_Make(m, def);

    Serve_AddFd(sctx, 0);
    Serve_Run(sctx);

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
