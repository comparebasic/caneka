#include <external.h>
#include <caneka.h>
#include <web-server.h>

static StrVec *headerPath = NULL;
static StrVec *footerPath = NULL;

static status Example_log(Step *_st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    Abstract *args[] = {
        (Abstract *)Lookup_Get(HttpMethods, ctx->method),
        (Abstract *)ctx->path,
        (Abstract *)MicroTime_ToStr(OutStream->m, MicroTime_Now()),
        NULL,
    };

    if(tsk->type.state & ERROR){
        Out("^r.Error $ @ $^0\n", args);
    }else{
        Out("^g.Served $ @ $^0\n", args);
    }
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    close(pfd->fd);

    return SUCCESS;
}

static status Example_PageContent(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);
    if(Route_Handle(rt, sm, (Object *)st->arg, NULL) & SUCCESS){
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    return st->type.state;
}

static status Example_FooterContent(Step *st, Task *tsk){
    Route *rt = Object_ByPath(tcp->inc, headerPath, NULL, SPAN_OP_GET);

    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);
    if(Route_Handle(rt, sm, (Object *)st->arg, NULL) & SUCCESS){
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    return st->type.state;
}

static status Example_HeaderContent(Step *st, Task *tsk){
    Route *rt = Object_ByPath(tcp->inc, footerPath, NULL, SPAN_OP_GET);

    Stream *sm = Stream_MakeToVec(tsk->m, ctx->content);
    if(Route_Handle(rt, sm, (Object *)st->arg, NULL) & SUCCESS){
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
    Route *rt = Object_ByPath(tcp->pages, ctx->path, NULL, SPAN_OP_GET);
    if(rt == NULL){
        st->type.state |= SUCCESS;
        ctx->code = 404;
        Abstract *args[] = {
            (Abstract *)ctx->path, (Abstract *)tcp->pages, NULL
        };
        Out("^b.Not Found @ -> &^0\n", args);
    }else{
        ctx->code = 200;
        Object *data = Object_Make(m, ZERO);
        Object_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)rt);
        Single *now = MicroTime_ToStr(tsk->m, MicroTime_Now());
        Object_Set(data, (Abstract *)Str_CstrRef(m, "now"), (Abstract *)now);

        ctx->content = StrVec_Make(tsk->m);
        Task_AddStep(tsk, Example_FooterContent, (Abstract *)data, NULL, ZERO);
        Task_AddStep(tsk, Example_PageContent, (Abstract *)data, NULL, ZERO);
        Task_AddStep(tsk, Example_FooterContent, (Abstract *)data, NULL, ZERO);
        st->type.state |= SUCCESS;
    }
    return st->type.state;
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
    r |= Dir_CheckCreate(m, Path_StrAdd(m, ctx->path, Str_CstrRef(m, "sessions")));
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

    headerPath = StrVec_From(m, Str_CstrRef(m, "/header.action"));
    IoUtil_Annotate(m, headerPath);
    footerPath = StrVec_From(m, Str_CstrRef(m, "/footerPath.action"));
    IoUtil_Annotate(m, footerPath);

    return r;
}

status WebServer_Run(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[2];
    
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
    Task_Tumble(tsk);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
