#include <external.h>
#include <caneka.h>
#include <rblsh.h>

static PatCharDef cCDef[] = {
    {PAT_TERM, 3, 3},
    {PAT_END, 0, 0},
};

static PatCharDef hupDef[] = {
    {PAT_TERM, 4, 4},
    {PAT_END, 0, 0},
};

#define sepKoDef {PAT_KO, ' ', ' '},{PAT_KO|PAT_INVERT_CAPTURE, '\n', '\n'},{PAT_KO|PAT_KO_TERM, '\t', '\t'}

static PatCharDef argDef[] = {
    sepKoDef,
    {PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''},{PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef flagDef[] = {
    sepKoDef,
    {PAT_TERM, '-', '-'},
    {PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''}, {PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef dblFlagDef[] = {
    sepKoDef,
    {PAT_TERM, '-', '-'},
    {PAT_TERM, '-', '-'},
    {PAT_INVERT|PAT_MANY, '"', '"'},{PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0},
};

static PatCharDef wsDef[] = {
    {PAT_MANY, ' ', ' '},{PAT_MANY|PAT_TERM, '\t', '\t'},
    {PAT_END, 0, 0},
};

static PatCharDef nlDef[] = {
    {PAT_TERM, '\n', '\n'},
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

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cCDef, RBLSH_TERM, RBLSH_MARK_END);
    mt = Roebling_LatestMatch(rbl);
    mt->type.state |= MATCH_SEARCH;

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)hupDef, RBLSH_HUP, RBLSH_MARK_END);
    mt = Roebling_LatestMatch(rbl);
    mt->type.state |= MATCH_SEARCH;

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)nlDef, RBLSH_NL, RBLSH_MARK_START);
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
    RblShCtx *ctx = (RblShCtx *)as(source, TYPE_RBLSH_CTX);
    printf("\x1b[%dmCaptured %s/\x1b[0m", COLOR_YELLOW, Class_ToString(captureKey));
    Debug_Print((void *)s, 0, "", COLOR_YELLOW, FALSE);
    Debug_Print((void *)ctx->shelf, 0, " shelf: ", COLOR_YELLOW, TRUE);
    printf("\n");

    if(captureKey == RBLSH_TERM || captureKey == RBLSH_HUP){
        exit(0);
    }

    if(captureKey >= _CASH_START && captureKey <= _CASH_END){
        Cash_Capture(captureKey, matchIdx, s, (Abstract *)ctx->cash);
    }

    return SUCCESS;
}

Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s){ 
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

    return Roebling_Make(m,
         TYPE_ROEBLING,
         parsers_do,
         desc,
         s,
         RblSh_Capture,
         (Abstract *)ctx); 
}
