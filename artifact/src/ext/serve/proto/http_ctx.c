#include <external.h>
#include <caneka.h>

ProtoCtx *HttpProto_Make(MemCh *m){
    ProtoCtx *ctx = ProtoCtx_Make(m);
    ctx->in = Buff_Make(m, BUFF_STRVEC);
    ctx->out = Buff_Make(m, BUFF_STRVEC);
    ctx->data = (Abstract *)HttpCtx_Make(m); 
    return ctx;
}

HttpCtx *HttpCtx_Make(MemCh *m){
    HttpCtx *ctx = (HttpCtx *)MemCh_AllocOf(m, sizeof(HttpCtx), TYPE_HTTP_CTX);
    ctx->type.of = TYPE_HTTP_CTX;
    ctx->headers = Table_Make(m);
    return ctx;
}
