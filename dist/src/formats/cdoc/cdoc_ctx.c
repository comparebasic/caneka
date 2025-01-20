#include <external.h>
#include <caneka.h>

FmtCtx *CdocCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CDOC;
    ctx->m = m;
    ctx->rbl = CdocCtx_RblMake(m, ctx);
    ctx->out = ToStdOut;
    /*

    ctx->rangeToChars = CnkLang_RangeToChars;
    CnkRblLang_AddDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->value = (Abstract *)Span_Make(m, TYPE_TABLE);
    ctx->item->spaceIdx = CNK_LANG_RBL_START;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    */
    ctx->source = source;

    return ctx;
}

