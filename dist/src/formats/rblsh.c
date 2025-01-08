#include <external.h>
#include <caneka.h>

static PatCharDef argDef[] = {
    {PAT_INVERT|PAT_MANY, '"', '"'}, {PAT_INVERT|PAT_MANY, '\'', '\''},{PAT_INVERT|PAT_MANY, ' ', ' '},{PAT_INVERT|PAT_MANY, '\t', '\t'}, {PAT_INVERT|PAT_MANY|PAT_MANY|PAT_TERM, 0, 31},
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
    return SUCCESS;
}

Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s, RblCaptureFunc capture){ 
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
         capture,
         (Abstract *)ctx); 
}
