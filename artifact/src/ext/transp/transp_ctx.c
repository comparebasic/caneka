#include <external.h>
#include <caneka.h>

TranspCtx *TranspCtx_Make(MemCh *m, Buff *bf, Lookup *lk){
    TranspCtx *ctx = (TranspCtx *)MemCh_Alloc(m, sizeof(TranspCtx));
    ctx->type.of = TYPE_TRANSP_CTX;
    ctx->m = m;
    ctx->bf = bf;
    ctx->lk = lk;
    ctx->stackIdx = -1;
    Iter_Init(&ctx->it, Span_Make(m));
    return ctx;
}
