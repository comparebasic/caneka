#include <external.h>
#include <caneka.h>
#include <signer.h>

SignerCtx *SignerCtx_Make(MemCtx *m){
    SignerCtx *ctx = MemCtx_Alloc(m, sizeof(SignerCtx));
    ctx->type.of = TYPE_SIGNER_CTX;
    ctx->m = m;
    ctx->identTbl = Span_Make(m, TYPE_TABLE);
    ctx->outputTbl = Span_Make(m, TYPE_TABLE);
    return ctx;
}
