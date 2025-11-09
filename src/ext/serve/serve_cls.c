#include <external.h>
#include <caneka.h>

static i64 ProtoCtx_Print(Buff *bf, void *a, cls type, word flags){
    ProtoCtx *ctx = (ProtoCtx*)as(a, TYPE_PROTO_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        Util_Wrapped(bf->m, ctx->u),
        ctx->in,
        ctx->out,
        ctx->data,
        NULL,
    };
    return Fmt(bf, "Proto<$ u:$ in:@ out:@ data:@>", args);
}

static i64 TcpCtx_Print(Buff *bf, void *a, cls type, word flags){
    TcpCtx *ctx = (TcpCtx*)as(a, TYPE_TCP_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        ctx->path,
        I32_Wrapped(bf->m, ctx->port),
        ctx->inet4,
        ctx->inet6,
        Util_Wrapped(bf->m, (util)ctx->metrics.start),
        Util_Wrapped(bf->m, (util)ctx->metrics.open),
        Util_Wrapped(bf->m, (util)ctx->metrics.error),
        Util_Wrapped(bf->m, (util)ctx->metrics.served),
        NULL,
    };
    return Fmt(bf, "Tcp<$ $ ^D.$^d.port ^D.$^d.inet4 ^D.$^d.inet6 $start $open $error $served>", args);
}

status Serve_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TCP_CTX, (void *)TcpCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTO_CTX, (void *)ProtoCtx_Print);
    return r;
}

status Serve_ClsInit(MemCh *m){
    return Serve_ToSInit(m, ToStreamLookup);
}
