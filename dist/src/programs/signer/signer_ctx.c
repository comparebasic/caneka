#include <external.h>
#include <caneka.h>
#include <signer.h>

static status SignerCtx_streamIdent(MemCtx *m, String *s, Abstract *source){
    Roebling *rbl = as(source, TYPE_ROEBLING);
    Roebling_Add(rbl, s);
    return Roebling_Run(rbl);
}

status SignerCtx_DigestIdent(SignerCtx *ctx){
    status r = READY;
    File file;
    File_Init(&file, File_GetCwdPath(ctx->m, ctx->configPath), NULL, NULL);
    file.abs = file.path;
    r |= File_Stream(ctx->m, &file, NULL, SignerCtx_streamIdent, (Abstract *)ctx->rbl);
    return r;
}

SignerCtx *SignerCtx_Make(MemCtx *m){
    SignerCtx *ctx = MemCtx_Alloc(m, sizeof(SignerCtx));
    ctx->type.of = TYPE_SIGNER_CTX;
    ctx->m = m;
    ctx->identTbl = Span_Make(m, TYPE_TABLE);
    ctx->outputTbl = Span_Make(m, TYPE_TABLE);
    return ctx;
}
