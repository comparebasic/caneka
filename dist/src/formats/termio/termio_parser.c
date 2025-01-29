#include <external.h>
#include <caneka.h>

PatCharDef betweenDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '\x1b', '\x1b'},
    patText,
    {PAT_END, 0, 0},
};

PatCharDef textDef[] = {
    patText,
    {PAT_END, 0, 0},
};

PatCharDef ansiStartDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '[', '['},
    {PAT_END, 0, 0},
};

PatCharDef ansiModeDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '[', '['},
    {PAT_SINGLE, '?', '?'}, {PAT_TERM, '=', '='},
    {PAT_END, 0, 0},
};

PatCharDef ansiNumDef[] = {
    {PAT_TERM|PAT_MANY, '0', '9'},
    {PAT_END, 0, 0},
};

PatCharDef ansiSepDef[] = {
    {PAT_TERM|PAT_MANY, ';', ';'},
    {PAT_END, 0, 0},
};

PatCharDef ansiDirectDef[] = {
    {PAT_TERM, '7', '8'},
    {PAT_END, 0, 0},
};


PatCharDef ansiCmdDef[] = {
    {PAT_SINGLE, 'A', 'H'}, {PAT_SINGLE, 'J', 'K'}, {PAT_SINGLE, 'h', 'h'}, {PAT_SINGLE, 'l', 'l'},  {PAT_SINGLE, 's', 's'}, {PAT_SINGLE, 'u', 'u'}, {PAT_TERM, 'm', 'm'},
    {PAT_END, 0, 0},
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)betweenDef, TERMIO_TEXT, TERMIO_CMD);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)textDef, TERMIO_TEXT, TERMIO_START);

    return r;
}

static status cmdType(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiModeDef, TERMIO_MODE, TERMIO_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiStartDef, TERMIO_MODE, TERMIO_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiDirectDef, TERMIO_DIRECT, TERMIO_START);

    return r;
}

static status values(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiNumDef, TERMIO_NUM, TERMIO_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiSepDef, TERMIO_SEP, TERMIO_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)ansiCmdDef, TERMIO_CMD, TERMIO_VALUE);

    return r;
}

Roebling *TermIo_RblMake(MemCtx *m, String *s, Abstract *source, RblCaptureFunc capture){
    Span *parsers =  Span_Make(m, TYPE_SPAN); 
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, TERMIO_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, TERMIO_CMD_TYPE));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)cmdType));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, TERMIO_VALUE));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)values));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, TERMIO_END));

    LookupConfig config[] = {
        {TERMIO_START, (Abstract *)String_Make(m, bytes("START"))},
        {TERMIO_CMD_TYPE, (Abstract *)String_Make(m, bytes("CMD_TYPE"))},
        {TERMIO_VALUE, (Abstract *)String_Make(m, bytes("CMD_VALUE"))},
        {TERMIO_END, (Abstract *)String_Make(m, bytes("CMD_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        capture,
        source
    ); 
}
