#include <external.h>
#include <caneka.h>

ProtoCtx *ProtoCtx_Make(MemCh *m){
    ProtoCtx *ctx = (ProtoCtx *)MemCh_AllocOf(m, sizeof(ProtoCtx), TYPE_PROTO);
    ctx->type.of = TYPE_PROTO;
    return ctx;
}
