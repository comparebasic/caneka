#include <external.h>
#include <caneka.h>

static word typeDef[] = {
    PAT_KO, '\n', '\n',
    PAT_TERM, '#', '#',
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'y', 'y',
    PAT_TERM, 'p', 'p',
    PAT_TERM, 'e', 'e',
    PAT_ANY, ' ',' ', PAT_ANY'\t' PAT_ANY|PAT_TERM,'\r', '\r',
    PAT_END, 0, 0
};

static word cDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word cEndDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word lineDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, CNK_LANG_LINE, CNK_LANG_START);

    return r;
}

CnkLangCtx *CnkLangCtx_Make(MemCtx *m){
    CnkLangCtx *ctx = CnkLangCtx_Make(m);
    ctx->type.of = TYPE_CNK_LANG;


    Span *parsers = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(mh, (DoFunc)start));

    LookupConfig config[] = {
        {CNK_LANG_START, (Abstract *)String_Make(m, bytes("START"))},
        {CNK_LANG_LINE, (Abstract *)String_Make(m, bytes("LINE"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    ctx->rbl = Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        Capture,
        (Abstract *)ctx
    ); 
    /*
    ctx->item = FmtItem_Make(m, ctx);
    */
    return ctx;
}
