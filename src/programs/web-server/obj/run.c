#include <external.h>
#include <caneka.h>
#include <web-server.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;
static Span *_sepSpan = NULL;

static status Example_errorPopulate(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Route *rt = Object_ByPath(tcp->pages, ctx->path, NULL, SPAN_OP_GET);
    Object *data = Example_getPageData(tsk, rt);

    Task_ResetChain(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddDataStep(tsk,
        Example_FooterContent, NULL, (Abstract *)data, (Abstract *)tsk, ZERO);
    Task_AddStep(tsk, Example_ServeError, arg, source, ZERO);
    Task_AddDataStep(tsk,
        Example_HeaderContent, (Abstract *)rt, (Abstract *)data, (Abstract *)tsk, ZERO);

    tsk->type.state |= TcpTask_ExpectSend(NULL, tsk);

    Out("^b.errorPopulate^0\n", NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status Example_log(Step *_st, Task *tsk){
    DebugStack_Push(_st, _st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Abstract *args[6];
    args[0] = (Abstract *)Lookup_Get(HttpMethods, ctx->method);
    args[1] = (Abstract *)I32_Wrapped(tsk->m, ctx->code);
    args[2] = (Abstract *)ctx->path;
    args[3] = NULL;
    args[4] = NULL;

    if(ctx->type.state & ERROR){
        args[4] = (Abstract *)ctx->errors;
        args[5] = NULL;
        Out("^r.Error $/$ @ $ @^0\n", args);
    }else if(ctx->code == 200){
        Out("^g.Served $/$ @ $^0\n", args);
    }else{
        Out("^c.Responded $/$ @ ^{TIME.human}^0\n", args);
    }
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    close(pfd->fd);

    Out("^b.log^0\n", NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status Example_populate(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, arg, source);
    Task_AddStep(tsk, Example_ServePage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    Out("^b.populate^0\n", NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    StrVec *path = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/public"));
    StrVec *abs = IoUtil_AbsVec(m, public);
    ctx->pages = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->pages, abs);

    path = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/inc"));
    abs = IoUtil_AbsVec(m, public);
    ctx->inc = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->inc, abs);

    return r;
}

static status setErrorHandler(Memch *m){
    Single *funcW = Func_Wrapped(m, Example_errorPopulate);
    Single *key = Util_Wrapped(ErrStream->m, (util)tsk);
    return Table_Set(TaskErrorHandlers, (Abstract *)key, (Abstract *)funcW);
}

static TcpCtx *tcpCtx_Make(MemCh *m, StrVec *path, i32 port, quad ip4, util ip6[2]){
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->populate = Example_populate;
    ctx->finalize = Example_log;
    ctx->path = path;
    return ctx;
}

status WebServer_Run(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[2];
    
    MemCh *m = MemCh_Make();

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "examples/web-server"));
    util ip6[2] = {0, 0};

    TcpCtx *ctx = tcpCtx_Make(m, path, 3000, 0, &ip6);
    routeInit(m, ctx);
    Task *tsk = ServeTcp_Make(ctx);

    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^y.Serving &\n", args);

    Task_Tumble(tsk);

    MemCh_Free(m);
    MemCh_Free(gm);
    DebugStack_Pop();
    return r;
}
