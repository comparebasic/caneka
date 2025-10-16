#include <external.h>
#include <caneka.h>

static boolean _httpInitialized = FALSE;
Lookup *HttpMethods = NULL;

static status HttpInit(MemCh *m){
    if(!_httpInitialized){
        _httpInitialized = TRUE;
        HttpMethods = Lookup_Make(m, HTTP_METHOD);
        Lookup_Add(m, HttpMethods, HTTP_METHOD_GET, Str_CstrRef(m, "GET"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_POST, Str_CstrRef(m, "POST"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_UPDATE, Str_CstrRef(m, "UPDATE"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_DELETE, Str_CstrRef(m, "DELETE"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_PUT, Str_CstrRef(m, "PUT"));
        return SUCCESS;
    }
    return NOOP; 
}
static i64 HttpCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    HttpCtx *ctx = (HttpCtx*)as(a, TYPE_HTTP_CTX);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)ctx, ToS_FlagLabels),
        (Abstract *)Lookup_Get(HttpMethods, ctx->method),
        (Abstract *)ctx->path,
        (Abstract *)ctx->headers,
        (Abstract *)ctx->body,
        NULL,
    };
    return Fmt(sm, "Http<$ $ $ headers:@ body:@>", args);
}

static i64 ProtoCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    ProtoCtx *ctx = (ProtoCtx*)as(a, TYPE_PROTO_CTX);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)ctx, ToS_FlagLabels),
        (Abstract *)Util_Wrapped(sm->m, ctx->u),
        (Abstract *)ctx->in,
        (Abstract *)ctx->out,
        (Abstract *)ctx->data,
        NULL,
    };
    return Fmt(sm, "Proto<$ u:$ in:@ out:@ data:@>", args);
}

static i64 TcpCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    TcpCtx *ctx = (TcpCtx*)as(a, TYPE_TCP_CTX);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)ctx, ToS_FlagLabels),
        (Abstract *)ctx->path,
        (Abstract *)I32_Wrapped(sm->m, ctx->port),
        (Abstract *)ctx->inet4,
        (Abstract *)ctx->inet6,
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.start),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.open),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.error),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.served),
        NULL,
    };
    return Fmt(sm, "Tcp<$ $ ^D.$^d.port ^D.$^d.inet4 ^D.$^d.inet6 $start $open $error $served>", args);
}

status Serve_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TCP_CTX, (void *)TcpCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTO_CTX, (void *)ProtoCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_CTX, (void *)HttpCtx_Print);
    return r;
}

status Serve_ClsInit(MemCh *m){
    HttpInit(m);
    return Serve_ToSInit(m, ToStreamLookup);
}
