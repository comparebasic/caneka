#include <external.h>
#include <caneka.h>

ProtoCtx *HttpProto_Make(MemCh *m){
    ProtoCtx *ctx = ProtoCtx_Make(m);
    ctx->in = Cursor_Make(m, StrVec_Make(m));
    ctx->out = Cursor_Make(m, StrVec_Make(m));
    ctx->data = (Abstract *)HttpCtx_Make(m); 
    return ctx;
}

HttpCtx *HttpCtx_Make(MemCh *m){
    HttpCtx *ctx = (HttpCtx *)MemCh_AllocOf(m, sizeof(HttpCtx), TYPE_HTTP_CTX);
    ctx->type.of = TYPE_HTTP_CTX;
    ctx->headers = Table_Make(m);
    return ctx;
}
