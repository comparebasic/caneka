#include <external.h>
#include <caneka.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;
static Span *_sepSpan = NULL;

static  Object *WebServer_getPageData(Task *tsk, Route *rt){
    return Object_Make(tsk->m, ZERO);
}

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

    StrVec *path = StrVec_From(m, Str_FromCstr(m, "/system/error", STRING_COPY));
    IoUtil_Annotate(m, path);
    ctx->path = path;
    ctx->code = 500;

    Task_ResetChain(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, data, NULL, ZERO);

    DebugStack_Pop();
    return SUCCESS;
}

static status WebServer_populate(MemCh *m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    pfd = TcpTask_GetPollFd(tsk);
    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, WebServer_ServePage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    DebugStack_Pop();
    return SUCCESS;
}

static status setErrorHandler(MemCh *m, Task *tsk){
    Single *funcW = Func_Wrapped(m, WebServer_errorPopulate);
    Single *key = Util_Wrapped(ErrStream->m, (util)tsk);
    return Table_Set(TaskErrorHandlers, key, funcW);
}

static TcpCtx *tcpCtx_Make(MemCh *m,
        StrVec *path, Object *pageBase, i32 port, quad ip4, util ip6[2]){
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = port;
    ctx->populate = WebServer_populate;
    ctx->finalize = WebServer_logAndClose;
    ctx->path = path;
    ctx->pageBase = pageBase;
    return ctx;
}

status WebServer_ServePage(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    Abstract *args[5];
    MemCh *m = tsk->m;
    status r = READY;

    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    DebugStack_SetRef(ctx->path, ctx->path->type.of);
    IoUtil_Annotate(tsk->m, ctx->path);
    Route *handler = Route_GetHandler(tcp->pages, ctx->path);

    Object *data = (Object *)st->data;
    if(data == NULL){
        data = Object_Make(m, ZERO);
    } 

    Object *nav = Object_Make(m, ZERO);
    Object_Set(nav,
        (Abstract *)Str_FromCstr(m, "pages", STRING_COPY), (Abstract *)tcp->pages);
    Object_Set(data,
        (Abstract *)Str_FromCstr(m, "nav", STRING_COPY), (Abstract *)nav);

    if(handler == NULL){
        Str *s = Str_FromCstr(m, "not found", STRING_COPY);

        ctx->contentLength = s->length;
        ctx->code = 404;
        ctx->mime = Str_FromCstr(m, "text/plain", STRING_COPY);

        Buff *bf = Buff_Make(m, ZERO);
        Buff_AddBytes(bf, s->bytes, s->length);
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
        HttpCtx_PrepareResponse(ctx, tsk);

        st->type.state |= MORE;

    }else{

        Object *config = (Object *)Object_GetPropByIdx(handler, ROUTE_PROPIDX_DATA);
        if(config != NULL){
            Object_Set(data,
                (Abstract *)Str_FromCstr(m, "config", STRING_COPY), (Abstract *)config);
        }

        ctx->mime = (Str *)Object_GetPropByIdx(handler, ROUTE_PROPIDX_MIME);
        Single *funcW = (Single *)as(
            Object_GetPropByIdx(handler, ROUTE_PROPIDX_FUNC),
            TYPE_WRAPPED_FUNC
        );

        Buff *headBf = NULL;
        if((funcW->type.state & ROUTE_ASSET) == 0){
            StrVec *path = StrVec_From(m, Str_FromCstr(m, "header", ZERO));
            Route *header = Object_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);
            headBf = Buff_Make(m, ZERO);
            r |= Route_Handle(header, headBf, data, NULL);
            Buff_Stat(headBf);
            ctx->contentLength += headBf->st.st_size;
        }

        Buff *bf = Buff_Make(m, ZERO);
        r |= Route_Handle(handler, bf, data, NULL);
        Buff_Stat(bf);
        ctx->contentLength += bf->st.st_size;

        Buff *footerBf = NULL;
        if((funcW->type.state & ROUTE_ASSET) == 0){
            StrVec *path = StrVec_From(m, Str_FromCstr(m, "footer", ZERO));
            Route *header = Object_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);

            footerBf = Buff_Make(m, ZERO);
            r |= Route_Handle(header, footerBf, data, NULL);
            Buff_Stat(footerBf);
            ctx->contentLength += footerBf->st.st_size;

        }

        ctx->code = 200;

        if(footerBf != NULL){
            Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)footerBf, NULL, ZERO);
        }
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
        if(headBf != NULL){
            Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)headBf, NULL, ZERO);
        }
        HttpCtx_PrepareResponse(ctx, tsk);
        st->type.state |= MORE;
    }

    st->type.state |= SUCCESS;
    DebugStack_Pop();
    return st->type.state;
}

status WebServer_LayRoute(MemCh *m, Route *pages, StrVec *dir, StrVec *path, boolean overlay){
    Route *rt = Route_From(m, dir);
    Route *rel = Route_Make(m);
    Object_ByPath(rel, path, rt, SPAN_OP_SET);
    return Object_Lay(pages, rel, pages->objType.of, overlay);
}

status WebServer_AddRoute(MemCh *m, Route *pages, StrVec *dir, StrVec *path){
    status r = READY;
    Route *rt = Route_From(m, dir);
    Object_ByPath(pages, path, rt, SPAN_OP_SET);
    return r;
}

Task *WebServer_Make(StrVec *path, Object *pageBase, i32 port, quad ip4, util *ip6){
    DebugStack_Push(NULL, 0);
    status r = READY;

    Task *tsk = ServeTcp_Make(NULL);
    tsk->source = (Abstract *)tcpCtx_Make(tsk->m, path, pageBase, port, ip4, ip6);
    setErrorHandler(tsk->m, tsk);

    DebugStack_Pop();
    return tsk;
}
