#include <external.h>
#include <caneka.h>

PatCharDef keyDef[] = {
    {PAT_KO|PAT_KO_TERM, ':', ':'},
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'}, {PAT_ANY|PAT_INVERT_CAPTURE, '\r', '\r'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_END, '\n', '\n'},
    patText,
    {PAT_END, 0, 0},
};

PatCharDef valueDef[] = {
    {PAT_KO|PAT_KO_TERM, ';', ';'},
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'}, {PAT_ANY|PAT_INVERT_CAPTURE, '\r', '\r'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_END, '\n', '\n'},
    {PAT_INVERT, ':', ':'}, patText,
    {PAT_END, 0, 0},
};

PatCharDef contentDef[] = {
    patText,
    {PAT_END, 0, 0},
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, keyDef, KVE_KEY, KVE_VALUE);
    r |= Roebling_SetPattern(rbl, valueDef, KVE_KVALUE, KVE_START);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, valueDef, KVE_KVALUE, KVE_START);
    return r;
}

static status content(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    if(rbl->length <= 0){
        return ERROR;
    }
    r |= Roebling_SetPattern(rbl, valueDef, KVE_CONTENT, KVE_START);
    Match *mt = Roebling_LatestMatch(rbl);
    mt->remaining = rbl->length;
    return r;
}

Roebling *Kve_RblMake(MemCtx *m, String *s, Abstract *source, RblCaptureFunc capture){
    Span *parsers_do =  Span_Make(m, TYPE_SPAN);
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, KVE_START));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, KVE_VALUE));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)value));
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, KVE_CONTENT));
    Span_Add(parsers_do, (Abstract *)Do_Wrapped(m, (DoFunc)content));
    Span_Add(parsers_do, (Abstract *)Int_Wrapped(m, KVE_END));

    LookupConfig config[] = {
        {KVE_START, (Abstract *)String_Make(m, bytes("START"))},
        {KVE_VALUE, (Abstract *)String_Make(m, bytes("VALUE"))},
        {KVE_CONTENT, (Abstract *)String_Make(m, bytes("CONTENT"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    if(s == NULL){
        s = String_Init(m, STRING_EXTEND);
    }

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers_do,
        desc,
        s,
        capture,
        source 
    ); 
}
