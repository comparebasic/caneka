#include <external.h>
#include <caneka.h>

TcpCtx *TcpCtx_Make(MemCh *m){
    TcpCtx *ctx = (TcpCtx *)MemCh_AllocOf(m, sizeof(TcpCtx), TYPE_TCP_CTX);
    ctx->type.of = TYPE_TCP_CTX;
    return ctx;
}
