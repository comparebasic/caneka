#include <external.h>
#include <caneka.h>

static status Cdoc_AddDefs(FmtCtx *ctx){
    Lookup *lk = Lookup_Make(ctx->m, _CDOC_START, NULL, NULL);
    FmtDef *def = NULL;

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_START;
    Lookup_Add(ctx->m, lk, def->id, def); 

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_COMMENT;
    Lookup_Add(ctx->m, lk, def->id, def); 

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_TYPE;
    Lookup_Add(ctx->m, lk, def->id, def); 

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_THING;
    Lookup_Add(ctx->m, lk, def->id, def); 

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_WS;
    Lookup_Add(ctx->m, lk, def->id, def); 

    def = FmtDef_Make(ctx->m);
    def->id = CDOC_END;
    Lookup_Add(ctx->m, lk, def->id, def); 

    return Fmt_Add(ctx->m, ctx, lk);
}

static char *Cdoc_RangeToChars(word range){
    printf("range to chars\n");
    return "UNKNOWN";
}

static status Cdoc_Setup(FmtCtx *ctx, Transp *p){
    DebugStack_Push(p, p->type.of);
    status r = READY;
    Target *t = Target_Make(ctx->m, p->dist, p->source->fname, String_Make(ctx->m, bytes(".fmt")));
    r |= Lookup_Add(ctx->m, p->targets, CDOC_TARGET_FMT, (Abstract *)t);
    p->targets->values->metrics.selected = p->targets->values->metrics.set;
    DebugStack_Pop();
    return r;
}

FmtCtx *CdocCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = FmtCtx_Make(m, ToStdOut);
    ctx->type.of = TYPE_LANG_CDOC;
    ctx->m = m;
    ctx->rbl = Cdoc_RblMake(m, ctx);
    ctx->rangeToChars = Cdoc_RangeToChars;
    ctx->Setup = Cdoc_Setup;

    Cdoc_AddDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->value = (Abstract *)Span_Make(m, TYPE_TABLE);
    ctx->item->spaceIdx = CDOC_START;
    FmtDef *def = Chain_Get(ctx->resolver->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;

    return ctx;
}

