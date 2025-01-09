#include <external.h>
#include <caneka.h>

static PatCharDef cCDef[] = {
    {PAT_TERM, 3, 3},
    {PAT_END, 0, 0},
};

static PatCharDef hupDef[] = {
    {PAT_TERM, 4, 4},
    {PAT_END, 0, 0},
};

static PatCharDef argDef[] = {
    {PAT_KO, ' ', ' '},{PAT_KO|PAT_KO_TERM, '\t', '\t'},{PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''},{PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef flagDef[] = {
    {PAT_TERM, '-', '-'},
    {PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''},{PAT_INVERT|PAT_MANY, ' ', ' '},{PAT_INVERT|PAT_MANY, '\t', '\t'}, {PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef dblFlagDef[] = {
    {PAT_TERM, '-', '-'},
    {PAT_TERM, '-', '-'},
    {PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''},{PAT_INVERT|PAT_MANY, ' ', ' '},{PAT_INVERT|PAT_MANY, '\t', '\t'}, {PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef wsDef[] = {
    {PAT_MANY, ' ', ' '},{PAT_MANY|PAT_TERM, '\t', '\t'},
    {PAT_END, 0, 0},
};

static PatCharDef litDef[] = {
    {PAT_TERM, '\'', '\''},
    {PAT_KO|PAT_KO_TERM, '\'', '\''}, patTextDef,
    {PAT_END, 0, 0},
};

static PatCharDef opDef[] = {
    {PAT_MANY, '>', '>'},
    {PAT_MANY, '<', '<'},
    {PAT_MANY, '&', '&'},
    {PAT_MANY, '~', '~'},
    {PAT_MANY, '*', '*'},
    {PAT_MANY, '=', '='},
    {PAT_MANY, '+', '+'},
    {PAT_MANY|PAT_TERM, '|', '|'},
    {PAT_END, 0, 0},
};

static PatCharDef cashStartDef[] = {
    {PAT_TERM, '"', '"'},
    {PAT_END, 0, 0},
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches);
    r |= Match_SetPattern(mt, cCDef);
    mt->captureKey = RBLSH_TERM;
    mt->jump = Roebling_GetMarkIdx(rbl, RBLSH_MARK_END);
    mt->type.state |= SEARCH;

    mt = Span_ReserveNext(rbl->matches);
    r |= Match_SetPattern(mt, hupDef);
    mt->captureKey = RBLSH_HUP;
    mt->jump = Roebling_GetMarkIdx(rbl, RBLSH_MARK_END);
    mt->type.state |= SEARCH;

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)argDef, RBLSH_ARG, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)flagDef, RBLSH_FLAG, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)dblFlagDef, RBLSH_WORDFLAG, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)wsDef, RBLSH_WS, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)opDef, RBLSH_OP, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)litDef, RBLSH_STRING_LIT, RBLSH_MARK_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cashStartDef, CASH_MARK_START, CASH_MARK_START);

    return SUCCESS;
}

static status RblSh_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    Stack(bytes("RblSh_Capture"), (Abstract *)s);
    RblShCtx *ctx = (RblShCtx *)as(source, TYPE_RBLSH_CTX);
    Debug_Print((void *)s, 0, "Captured: ", COLOR_YELLOW, FALSE);
    printf("\n");

    if(captureKey == RBLSH_TERM || captureKey == RBLSH_HUP){
        exit(0);
    }

    if(captureKey >= _CASH_START && captureKey <= _CASH_END){
        Cash_Capture(captureKey, matchIdx, s, (Abstract *)ctx->cash);
    }

    Return SUCCESS;
}

Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s){ 
    Stack(bytes("RblShParser_Make"), NULL);
    Span *parsers_do =  Span_Make(m, TYPE_SPAN); 
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBLSH_MARK_START));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, CASH_MARK_START));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)Cash_Between));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)Cash_ValueQuoted));
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, RBLSH_MARK_END));

    LookupConfig config[] = {
        {_APP_BOUNDRY_START, (Abstract *)String_Make(m, bytes("APP_START"))},
        {RBLSH_MARK_START, (Abstract *)String_Make(m, bytes("START"))},
        {RBLSH_MARK_END, (Abstract *)String_Make(m, bytes("END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    Return Roebling_Make(m,
         TYPE_ROEBLING,
         parsers_do,
         desc,
         s,
         RblSh_Capture,
         (Abstract *)ctx); 
}
