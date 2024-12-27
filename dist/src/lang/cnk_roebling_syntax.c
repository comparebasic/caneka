#include <external.h>
#include <caneka.h>

word openCloseDef[] = {
    PAT_TERM, '/', '/',
    PAT_INVERT|PAT_INVERT_CAPTURE, '/', '/',
    PAT_END, 0, 0,
};

word patKeyDef[] = {
    PAT_KO|PAT_KO_TERM, '(', '(',
    patText,
    PAT_END, 0, 0,
};

word patDef[] = {
    PAT_KO|PAT_KO_TERM, '{', '{',
    patText,
    PAT_END, 0, 0,
};

word textDef[] = {
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '{', '{', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '[', '[', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '/', '/', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, ' ', ' ', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '\t', '\t', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '\r', '\r', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE|PAT_KO_TERM, '\n', '\n', 
    patText,
    PAT_END, 0, 0,
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    
    r |= Roebling_SetPattern(rbl, (PatCharDef *)openCloseDef, 
        CNK_LANG_RBL_OPEN_CLOSE,  
        CNK_LANG_RBL_PAT);

    return r;
}

static status pat(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    
    r |= Roebling_SetPattern(rbl, (PatCharDef *)patKeyDef, 
        CNK_LANG_RBL_PAT_KEY,  
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)textDef, 
        CNK_LANG_RBL_TEXT,
        CNK_LANG_RBL_PAT);

    return r;
}

status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_LANG_CNK_RBL);
    if(DEBUG_LANG_CNK_RBL){
        printf("\x1b[%dmCnkRbl Capture %s:", DEBUG_LANG_CNK_RBL, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " ", DEBUG_LANG_CNK_RBL, TRUE);
        printf("\n");
    }
    return NOOP;
}

status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks){
    Span *additions = Span_Make(m, TYPE_SPAN);
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_START));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_PAT));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)pat));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_END));

    LookupConfig config[] = {
        {CNK_LANG_RBL_START, (Abstract *)String_Make(m, bytes("RBL_START"))},
        {CNK_LANG_RBL_PAT, (Abstract *)String_Make(m, bytes("RBL_PAT"))},
        {CNK_LANG_RBL_END, (Abstract *)String_Make(m, bytes("RBL_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    return Roebling_AddParsers(m, parsers, marks, additions,  desc);
}

Roebling *CnkRoeblingCtx_RblMake(MemCtx *m, FmtCtx *ctx){
    Span *parsers = Span_Make(m, TYPE_SPAN);
    Lookup *desc = Lookup_Make(m, CNK_LANG_RBL_START, NULL, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    CnkRoebling_AddParsers(m, parsers, desc);

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        CnkRoebling_Capture,
        (Abstract *)ctx
    ); 
}

FmtCtx *CnkRoeblingCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK_RBL;
    ctx->m = m;
    ctx->rbl = CnkRoeblingCtx_RblMake(m, ctx);
    ctx->out = ToStdOut;

    ctx->rangeToChars = CnkLang_RangeToChars;
    /*
    CnkRblLang_AddDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->item->spaceIdx = CNK_LANG_ROEBLING;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;
    ctx->start = CnkLangCtx_Start;
    */

    return ctx;
}
