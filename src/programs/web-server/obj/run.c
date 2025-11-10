#include <external.h>
#include <caneka.h>
#include <web-server.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;
static Span *_sepSpan = NULL;

static  Object *Example_getPageData(Task *tsk, Route *rt){
    return Object_Make(tsk->m, ZERO);
}

static status Example_logAndClose(Step *_st, Task *tsk){
    DebugStack_Push(_st, _st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    MemBookStats st;
    MemBook_GetStats(tsk->m, &st);
    void *args[8];
    args[0] = Lookup_Get(HttpMethods, ctx->method);
    args[1] = I32_Wrapped(tsk->m, ctx->code);
    args[2] = ctx->path;
    args[3] = Str_MemCount(tsk->m, st.total * PAGE_SIZE),
    args[4] = I64_Wrapped(tsk->m, st.total),
    args[5] = I64_Wrapped(tsk->m, st.pageIdx),
    args[6] = NULL;
    args[7] = NULL;

    if(ctx->type.state & ERROR){
        args[4] = ctx->errors;
        args[5] = NULL;
        Out("^r.Error $/$ @ $ @ $ $/$ ^{TIME.human}^0\n", args);
    }else if(ctx->code == 200){
        Out("^g.Served $/$ @ $ $/$ ^{TIME.human}^0\n", args);
    }else{
        Out("^c.Responded $/$ @ $ $/$ ^{TIME.human}^0\n", args);
    }
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    close(pfd->fd);

    DebugStack_Pop();
    return SUCCESS;
}

static status Example_errorPopulate(MemCh *_m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);

    printf("Error Populate\n");
    fflush(stdout);

    MemCh *m = tsk->m; 
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    ErrorMsg *msg = (ErrorMsg *)as(arg, TYPE_ERROR_MSG);

    Object *data = Object_Make(m, TYPE_OBJECT);
    Object *error = Object_Make(m, TYPE_OBJECT);
    Object_Set(error,
        Str_FromCstr(m, "name", STRING_COPY),
        msg->lineInfo[0]);

    Buff *bf = Buff_Make(m, ZERO);
    Fmt(bf, (char *)msg->fmt->bytes, msg->args);
    Object_Set(data,
        Str_FromCstr(m, "details", STRING_COPY), 
        bf->v);

    StrVec *path = StrVec_From(m, Str_FromCstr(m, "/error.templ", STRING_COPY));
    IoUtil_Annotate(m, path);
    ctx->path = path;
    ctx->code = 500;

    Task_ResetChain(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, data, NULL, ZERO);

    DebugStack_Pop();
    return SUCCESS;
}

static status Example_populate(MemCh *m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    pfd = TcpTask_GetPollFd(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, Example_ServePage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    StrVec *path = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/public"));
    StrVec *abs = IoUtil_AbsVec(m, path);
    ctx->pages = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->pages, abs);

    path = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/inc"));
    abs = IoUtil_AbsVec(m, path);
    ctx->inc = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->inc, abs);

    return r;
}

static status setErrorHandler(MemCh *m, Task *tsk){
    Single *funcW = Func_Wrapped(m, Example_errorPopulate);
    Single *key = Util_Wrapped(ErrStream->m, (util)tsk);
    return Table_Set(TaskErrorHandlers, key, funcW);
}

static TcpCtx *tcpCtx_Make(MemCh *m, StrVec *path, i32 port, quad ip4, util ip6[2]){
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->populate = Example_populate;
    ctx->finalize = Example_logAndClose;
    ctx->path = path;
    return ctx;
}

status WebServer_Run(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[2];
    
    MemCh *m = MemCh_Make();

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "examples/web-server"));
    util ip6[2] = {0, 0};

    TcpCtx *ctx = tcpCtx_Make(m, path, 3000, 0, ip6);
    routeInit(m, ctx);
    Task *tsk = ServeTcp_Make(ctx);
    setErrorHandler(m, tsk);

    args[0] = ctx;
    args[1] = NULL;
    Out("^y.Serving &\n", args);

    Task_Tumble(tsk);

    MemCh_Free(m);
    MemCh_Free(gm);
    DebugStack_Pop();
    return r;
}
