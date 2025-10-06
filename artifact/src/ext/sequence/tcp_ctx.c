#include <external.h>
#include <caneka.h>

TcpCtx *TcpCtx_Make(MemCh *m, SourceMakerFunc mk, i32 port, Str *inet4, Str *inet6){
    TcpCtx *ctx = MemCh_AllocOf(m, sizeof(TcpCtx), TYPE_TCP_CTX);
    ctx->type.of = TYPE_TCP_CTX;
    ctx->port = port;
    ctx->inet4 = inet4;
    ctx->inet6 = inet6;
    ctx->mk = mk;
    return ctx;
}
