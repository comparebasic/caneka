#include <external.h>
#include <caneka.h>

TranspCtx *TranspCtx_Make(MemCh *m, Stream *sm, Lookup *lk){
    TranspCtx *ctx = (TranspCtx *)MemCh_Alloc(m, sizeof(TranspCtx));
    ctx->type.of = TYPE_TRANSP_CTX;
    ctx->m = m;
    ctx->sm = sm;
    ctx->lk = lk;
    Iter_Init(&ctx->it, Span_Make(m));
    return ctx;
}
