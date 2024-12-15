#include <external.h>
#include <caneka.h>

static addDefs(FmtCtx *ctx){
    status r = READY;

    Lookup *lk = Lookup_Make(ctx->m, CNK_LANG_START, NULL, NULL);
    r |= Lookup_Add(ctx->m, lk, 
    FmtDef *def = NULL;
    def = FmtDef_Make(m);
    def->id = CNK_LANG_STRUCT;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_STRUCT, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_REQUIRE;
    def->name = String_Make(m, bytes("require"));
    def->alias = String_Make(m, bytes("@"));
    def->to = CnkLang_RequireTo;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_REQUIRE, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_PACKAGE;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_PACKAGE, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_TYPE;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_TYPE, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_ARG_LIST;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_ARG_LIST,def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_C;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_C,
    def = FmtDef_Make(m);
    def->id = CNK_LANG_TOKEN;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_TOKEN, def);
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_TOKEN_DOT, def);
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_TOKEN_NULLABLE, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_ASSIGN;
    def = FmtDef_Make(m);
    def->name = String_Make(m, bytes("assign"));
    def->alias = String_Make(m, bytes("="));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_ASSIGN,def);
    def = FmtDef_Make(m);
    def->name = String_Make(m, bytes("is"));
    def->alias = String_Make(m, bytes("is"));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_IS,def);
    def = FmtDef_Make(m);
    def->name = String_Make(m, bytes("not"));
    def->alias = String_Make(m, bytes("not"));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_NOT,def);
    def = FmtDef_Make(m);
    def->name = String_Make(m, bytes("flagadd"));
    def->alias = String_Make(m, bytes("&"));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_FLAG_ADD,def);
    def->name = String_Make(m, bytes("flagsub"));
    def->alias = String_Make(m, bytes("~"));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_FLAG_SUB,def);
    def = FmtDef_Make(m);
    def->name = String_Make(m, bytes("rbl"));
    def->alias = String_Make(m, bytes("/"));
    def->id = CNK_LANG_ROEBLING;
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_ROEBLING, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_KEYS;
    def->name = String_Make(m, bytes("keys"));
    def->alias = String_Make(m, bytes("{"));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_KEYS, def);
    def = FmtDef_Make(m);
    def->id = CNK_LANG_IDXS;
    def->name = String_Make(m, bytes("idxs"));
    def->alias = String_Make(m, bytes("["));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_IDXS, def);
    def->name = String_Make(m, bytes("seq"));
    def->alias = String_Make(m, bytes("("));
    r |= Lookup_Add(ctx->m, lk, CNK_LANG_SEQ, def);

    return Fmt_Add(ctx->m, ctx, lk);
}


FmtCtx *CnkLangCtx_Make(MemCtx *m){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m);

    ctx->rangeToChars = cnkRangeToChars;
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->spaceIdx = CNK_LANG_START;

    return ctx;
}
