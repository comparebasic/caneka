#include <external.h>
#include <caneka.h>

static i64 HttpCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    HttpCtx *ctx = (HttpCtx*)as(a, TYPE_PROTO_CTX);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)ctx, ToS_FlagLabels),
        (Abstract *)ctx->body,
        (Abstract *)ctx->headers,
        NULL,
    };
    return Fmt(sm, "Http<$ headers:@ body:@>", args);
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
        (Abstract *)I32_Wrapped(sm->m, ctx->port),
        (Abstract *)ctx->inet4,
        (Abstract *)ctx->inet6,
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.start),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.open),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.error),
        (Abstract *)Util_Wrapped(sm->m, (util)ctx->metrics.served),
        NULL,
    };
    return Fmt(sm, "Tcp<$ u:$ inet4:$ inet6:$ maker:^D.$^d. $start $open $error $served>", args);
}

status Serve_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TCP_CTX, (void *)TcpCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTO_CTX, (void *)ProtoCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_CTX, (void *)HttpCtx_Print);
    return r;
}

status Serve_ClsInit(MemCh *m){
    return Serve_ToSInit(m, ToStreamLookup);
}
