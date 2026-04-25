#include <external.h>
#include <caneka.h>

IoCtx *IoCtx_Make(MemCh *m){
    IoCtx *ctx = (IoCtx *)MemCh_AllocOf(m, sizeof(IoCtx), TYPE_IO_CTX);
    ctx->type.of = TYPE_IO_CTX;
    return ctx;
}
