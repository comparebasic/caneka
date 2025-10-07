#include <external.h>
#include <caneka.h>

ProtoCtx *HttpCtx_Make(MemCh *m){
    ProtoCtx *ctx = ProtoCtx_Make(m);
    ctx->in = Cursor_Make(m, StrVec_Make(m));
    ctx->out = Cursor_Make(m, StrVec_Make(m));
    ctx->data = (Abstract *)Table_Make(m); 
    return ctx;
}
