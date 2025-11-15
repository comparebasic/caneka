#include <external.h>
#include <caneka.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;
static Span *_sepSpan = NULL;

static status WebServer_logAndClose(Step *_st, Task *tsk){
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

static status WebServer_errorPopulate(MemCh *_m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);

    MemCh *m = tsk->m; 
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    ErrorMsg *msg = (ErrorMsg *)as(arg, TYPE_ERROR_MSG);

    if(ctx->data == NULL){
        ctx->data = Table_Make(m);
    }

    Table_Set(ctx->data, S(m, "nav"), tcp->nav);

    Table *error = Table_Make(m);
    Table_Set(error, S(m, "name"), msg->lineInfo[0]);

    Buff *bf = Buff_Make(m, ZERO);
    Fmt(bf, (char *)msg->fmt->bytes, msg->args);
    Table_Set(error, S(m, "details"), bf->v);

    Table_Set(ctx->data, S(m, "error"), error);

    StrVec *path = Sv(m, "/system/error");
    IoUtil_Annotate(m, path);
    ctx->path = path;
    ctx->code = 500;

    Task_ResetChain(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, WebServer_ServePage, NULL, NULL, ZERO);

    DebugStack_Pop();
    return SUCCESS;
}

static status WebServer_populate(MemCh *m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, WebServer_GatherPage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status setErrorHandler(MemCh *m, Task *tsk){
    Single *funcW = Func_Wrapped(m, WebServer_errorPopulate);
    Single *key = Util_Wrapped(ErrStream->m, (util)tsk);
    return Table_Set(TaskErrorHandlers, key, funcW);
}

static TcpCtx *tcpCtx_Make(MemCh *m, i32 port, quad ip4, util ip6[2]){
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = port;
    ctx->populate = WebServer_populate;
    ctx->finalize = WebServer_logAndClose;
    return ctx;
}

status WebServer_GatherPage(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    void *args[5];

    MemCh *m = tsk->m;

    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    IoUtil_Annotate(tsk->m, ctx->path);
    ctx->route = Route_GetHandler(tcp->pages, ctx->path);

    ctx->data = Table_Make(m);
    Table_Set(ctx->data, S(m, "nav"), tcp->nav);

    if(ctx->route == NULL){
        ctx->code = 404;

        StrVec *path = ctx->path;
        Single *funcW = Route_MimeFunc(ctx->path);

        if(funcW != NULL && (funcW->type.state & ROUTE_ASSET) == 0){
            ctx->route = Route_GetHandler(tcp->pages, IoPath(m, "/system/not-found"));
            if(ctx->route != NULL){

                Table *routeData = Seel_Get(ctx->route, K(m, "data"));
                if(routeData != NULL && routeData->nvalues > 0){
                    Table_Set(ctx->data, S(m, "config"), Table_Get(routeData, K(m, "config")));
                }

                Table *error = Table_Make(m);
                Table_Set(error, S(m, "name"), S(m, "Page not found"));
                Table_Set(error, S(m, "details"), ctx->path);

                Table_Set(ctx->data, S(m, "error"), error);
                Task_AddStep(tsk, WebServer_ServePage, NULL, NULL, ZERO);

                st->type.state |= (MORE|SUCCESS);
                DebugStack_Pop();
                return st->type.state;
            }
        }

        ctx->mime = S(m, "text/plain");

        Str *s = S(m, "not found");
        ctx->contentLength = s->length;

        Buff *bf = Buff_Make(m, ZERO);
        Buff_AddBytes(bf, s->bytes, s->length);

        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
        HttpCtx_PrepareResponse(ctx, tsk);

        st->type.state |= (MORE|SUCCESS);
        DebugStack_Pop();
        return st->type.state;
    }

    Table *routeData = Seel_Get(ctx->route, K(m, "data"));
    if(routeData != NULL && routeData->nvalues > 0){
        Table_Set(ctx->data, S(m, "config"), Table_Get(routeData, K(m, "config")));
    }

    Task_AddStep(tsk, WebServer_ServePage, NULL, NULL, ZERO);

    Single *gatherFunc = Seel_Get(ctx->route, K(m, "addStep"));
    if(gatherFunc != NULL && gatherFunc->type.of == TYPE_WRAPPED_PTR){
        Task_AddStep(tsk, gatherFunc->val.ptr, NULL, NULL, ZERO);
    }

    st->type.state |= (MORE|SUCCESS);
    DebugStack_Pop();
    return st->type.state;
}

status WebServer_ServePage(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    void *args[5];
    MemCh *m = tsk->m;
    status r = READY;

    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    DebugStack_SetRef(ctx->path, ctx->path->type.of);

    ctx->mime = (Str *)Seel_Get(ctx->route, K(m, "mime"));
    Single *funcW = (Single *)as(
        Span_Get(ctx->route, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );

    Buff *headBf = NULL;
    if((funcW->type.state & ROUTE_ASSET) == 0){
        StrVec *path = Sv(m, "header");
        Route *header = Inst_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);
        headBf = Buff_Make(m, ZERO);
        r |= Route_Handle(header, headBf, ctx->data, NULL);
        Buff_Stat(headBf);
        ctx->contentLength += headBf->st.st_size;
    }

    Buff *bf = Buff_Make(m, ZERO);
    r |= Route_Handle(ctx->route, bf, ctx->data, NULL);
    Buff_Stat(bf);
    ctx->contentLength += bf->st.st_size;

    Buff *footerBf = NULL;
    if((funcW->type.state & ROUTE_ASSET) == 0){
        StrVec *path = Sv(m, "footer");
        Route *header = Inst_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);

        footerBf = Buff_Make(m, ZERO);
        r |= Route_Handle(header, footerBf, ctx->data, NULL);
        Buff_Stat(footerBf);
        ctx->contentLength += footerBf->st.st_size;
    }

    if(ctx->code == 0){
        ctx->code = 200;
    }

    if(footerBf != NULL){
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)footerBf, NULL, ZERO);
    }
    Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
    if(headBf != NULL){
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)headBf, NULL, ZERO);
    }

    HttpCtx_PrepareResponse(ctx, tsk);

    st->type.state |= (MORE|SUCCESS);
    DebugStack_Pop();
    return st->type.state;
}

Task *WebServer_Make(i32 port, quad ip4, util *ip6){
    DebugStack_Push(NULL, 0);
    status r = READY;

    Task *tsk = ServeTcp_Make(NULL);
    tsk->source = (Abstract *)tcpCtx_Make(tsk->m, port, ip4, ip6);
    setErrorHandler(tsk->m, tsk);

    DebugStack_Pop();
    return tsk;
}
