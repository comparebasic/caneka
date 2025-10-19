#include <external.h>
#include <caneka.h>
#include <web-server.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;
static Span *_sepSpan = NULL;

static Object *Example_getPageData(Task *tsk, Route *rt){
    MemCh *m = tsk->m;
    Object *data = Object_Make(m, ZERO);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)rt);
    Str *now = MicroTime_ToStr(tsk->m, MicroTime_Now());
    Object_Set(data, (Abstract *)Str_CstrRef(m, "now"), (Abstract *)now);
    StrVec *title = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_PATH);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

    return data;
}

static status Example_log(Step *_st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Abstract *args[6];
    args[0] = (Abstract *)Lookup_Get(HttpMethods, ctx->method);
    args[1] = (Abstract *)I32_Wrapped(tsk->m, ctx->code);
    args[2] = (Abstract *)ctx->path;
    args[3] = (Abstract *)MicroTime_ToStr(OutStream->m, MicroTime_Now());
    args[4] = NULL;

    if(ctx->type.state & ERROR){
        args[4] = (Abstract *)ctx->errors;
        args[5] = NULL;
        Out("^r.Error $/$ @ $ @^0\n", args);
    }else if(ctx->code == 200){
        Out("^g.Served $/$ @ $^0\n", args);
    }else{
        Out("^c.Responded $/$ @ $^0\n", args);
    }
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    close(pfd->fd);

    return SUCCESS;
}

static status Example_PageContent(Step *st, Task *tsk){
    Abstract *args[5];
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    Route *rt = (Route *)st->arg;

    if(Route_Handle(tsk->m, rt, ctx->content, (Object *)st->data, (Abstract *)tsk) & SUCCESS){
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    return st->type.state;
}

static status Example_FooterContent(Step *st, Task *tsk){
    Abstract *args[5];
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    Route *rt = Object_ByPath(tcp->inc, footerPath, NULL, SPAN_OP_GET);
    if(rt == NULL){
        args[0] = (Abstract *)tcp->inc;
        args[1] = NULL;
        Error(tsk->m, FUNCNAME, FILENAME, LINENUMBER,
            "Route is null for Footer: @", args);
        st->type.state |= ERROR;
        return st->type.state;
    }
    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);
    if(Route_Handle(tsk->m, rt, ctx->content, (Object *)st->data, NULL) & SUCCESS){
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    return st->type.state;
}

static status Example_HeaderContent(Step *st, Task *tsk){
    Abstract *args[3];
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    Route *rt = Object_ByPath(tcp->inc, headerPath, NULL, SPAN_OP_GET);
    if(rt == NULL){
        args[0] = (Abstract *)tcp->inc;
        args[1] = NULL;
        Error(tsk->m, FUNCNAME, FILENAME, LINENUMBER,
            "Route is null for Header: @", args);
        st->type.state |= ERROR;
        return st->type.state;
    }

    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);
    if(Route_Handle(tsk->m, rt, ctx->content, (Object *)st->data, NULL) & SUCCESS){
        ctx->mime = (Str *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_MIME);
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    return st->type.state;
}

static status Example_ServePage(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Path_Annotate(tsk->m, ctx->path, _sepSpan);
    Route *rt = Object_ByPath(tcp->pages, ctx->path, NULL, SPAN_OP_GET);
    if(rt == NULL){
        st->type.state |= SUCCESS;
        ctx->code = 404;
        Abstract *args[] = {
            (Abstract *)ctx->path, NULL
        };
        ctx->content = StrVec_Make(tsk->m);
        StrVec_Add(ctx->content, Str_CstrRef(tsk->m, "Page Not Found"));
        return st->type.state;
    }else{
        ctx->code = 200;

        Object *data = Example_getPageData(tsk, rt);

        ctx->content = StrVec_Make(tsk->m);
        if(rt->type.state & ROUTE_PAGE){
            Task_AddDataStep(tsk,
                Example_FooterContent, NULL, (Abstract *)data, (Abstract *)tsk, ZERO);
        }
        Task_AddDataStep(tsk,
            Example_PageContent, (Abstract *)rt, (Abstract *)data, (Abstract *)tsk, ZERO);
        if(rt->type.state & ROUTE_PAGE){
            Task_AddDataStep(tsk,
                Example_HeaderContent, (Abstract *)rt, (Abstract *)data, (Abstract *)tsk, ZERO);
        }

        st->type.state |= SUCCESS;
        return st->type.state|MORE;
    }
}

static status Example_ServeError(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    ctx->code = 500;
    ctx->type.state |= ERROR;
    ctx->mime = Str_CstrRef(tsk->m, "text/html");

    if(ctx->content == NULL){
        ctx->content = StrVec_Make(tsk->m);
    }
    StrVec_Add(ctx->content, Str_CstrRef(tsk->m,
        "<h1>Server Error</h1>\r\n    <p>"));
    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);

    ErrorMsg *msg = (ErrorMsg *)as(st->arg, TYPE_ERROR_MSG);
    if(ctx->errors == NULL){
        ctx->errors = Span_Make(tsk->m);
    }

    Span_Add(ctx->errors, (Abstract *)msg);
    ToS(sm, (Abstract *)msg, msg->type.of, ZERO);

    StrVec_Add(ctx->content, Str_CstrRef(tsk->m, "</p>\r\n"));
    st->type.state |= SUCCESS;
    return st->type.state;
}

static status Example_errorPopulate(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
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

    return SUCCESS;
}

static status Example_populate(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, arg, source);

    Task_AddStep(tsk, Example_ServePage, NULL, NULL, ZERO);

    HttpTask_AddRecieve(tsk, NULL, NULL);
    return SUCCESS;
}

static status serveInit(MemCh *m, TcpCtx *ctx){
    status r = READY;
    Abstract *args[5];

    if(_sepSpan == NULL){
        _sepSpan = Span_Make(m);
        Span_Add(_sepSpan, (Abstract *)B_Wrapped(m, (byte)'/', ZERO, MORE));
    }

    Str *sp = Path_StrAdd(m, ctx->path, Str_CstrRef(m, "sessions"));
    sp->type.state |= DEBUG;
    r |= Dir_CheckCreate(m, sp);
    r |= Dir_CheckCreate(m, Path_StrAdd(m, ctx->path, Str_CstrRef(m, "users")));
    r |= Dir_CheckCreate(m, Path_StrAdd(m, ctx->path, Str_CstrRef(m, "pages/data")));


    Str *public = Path_StrAdd(m, ctx->path, Str_CstrRef(m, "pages/public"));
    Str *inc = Path_StrAdd(m, ctx->path, Str_CstrRef(m, "pages/inc"));
    r |= Dir_CheckCreate(m, public);
    r |= Dir_CheckCreate(m, inc);

    ctx->pages = (Object *)Route_Make(m);
    Route_Collect((Route *)ctx->pages, StrVec_From(m, public));

    ctx->inc = (Object *)Route_Make(m);
    Route_Collect((Route *)ctx->inc, StrVec_From(m, inc));

    headerPath = StrVec_From(m, Str_CstrRef(m, "/header"));
    IoUtil_Annotate(m, headerPath);
    footerPath = StrVec_From(m, Str_CstrRef(m, "/footer"));
    IoUtil_Annotate(m, footerPath);

    return r;
}

status WebServer_Run(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[2];
    
    MemCh *m = MemCh_Make();
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->populate = Example_populate;
    ctx->finalize = Example_log;
    ctx->path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "examples/web-server"));

    serveInit(m, ctx);

    Task *tsk = ServeTcp_Make(ctx);
    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^y.Serving &\n", args);

    Single *funcW = Func_Wrapped(m, Example_errorPopulate);
    Single *key = Util_Wrapped(ErrStream->m, (util)tsk);
    Table_Set(TaskErrorHandlers, (Abstract *)key, (Abstract *)funcW);

    Task_Tumble(tsk);

    MemCh_Free(m);
    MemCh_Free(gm);
    DebugStack_Pop();
    return r;
}
