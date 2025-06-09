#include <external.h>
#include <caneka.h>

static PatCharDef commentDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '/', '/'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '*', '*'},
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, '\n', '\n'},
    {PAT_KO|PAT_KO_TERM, '*', '*'}, {PAT_KO|PAT_KO_TERM, '/', '/'}, patText,
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'},
    {PAT_ANY|PAT_INVERT_CAPTURE, '\r', '\r'},
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0},
};

static PatCharDef funcSigDef[] = {
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '*', '*'}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '(', '('},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, ')', ')'},
    {PAT_TERM, ';', ';'},
    {PAT_END, 0, 0},
};

static PatCharDef typeDef[] = {
    {PAT_TERM, 't', 't'},
    {PAT_TERM, 'y', 'y'},
    {PAT_TERM, 'p', 'p'},
    {PAT_TERM, 'e', 'e'},
    {PAT_TERM, 'd', 'd'},
    {PAT_TERM, 'e', 'e'},
    {PAT_TERM, 'f', 'f'},
    {PAT_TERM|PAT_MANY, ' ', ' '},
    {PAT_TERM, 's', 's'},
    {PAT_TERM, 't', 't'},
    {PAT_TERM, 'r', 'r'},
    {PAT_TERM, 'u', 'u'},
    {PAT_TERM, 'c', 'c'},
    {PAT_TERM, 't', 't'},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '{', '{'},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '}', '}'},
    {PAT_TERM|PAT_MANY, ' ', ' '},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'},
    {PAT_TERM|PAT_MANY, ' ', ' '},
    {PAT_TERM, ';', ';'},
    {PAT_END, 0, 0},
};

static PatCharDef wsDef[] = {
    {PAT_ANY, ' ', ' '},
    {PAT_ANY, '\t', '\t'},
    {PAT_ANY, '\r', '\r'},
    {PAT_ANY, '\n', '\n'},
    {PAT_END, 0, 0},
};

static PatCharDef otherDef[] = {
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    patText,
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'},
    {PAT_ANY|PAT_INVERT_CAPTURE, '\r', '\r'},
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0},
};


static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, commentDef, CDOC_COMMENT, CDOC_THING);
    Match *mt = Roebling_LatestMatch(rbl);
    mt->type.state |= MATCH_SEARCH;
    r |= Roebling_SetPattern(rbl, wsDef, CDOC_WS, CDOC_START);

    return r;
}

static status thing(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, funcSigDef, CDOC_THING, CDOC_START);
    r |= Roebling_SetPattern(rbl, typeDef, CDOC_TYPE, CDOC_START);
    r |= Roebling_SetPattern(rbl, commentDef, CDOC_COMMENT, CDOC_THING);
    r |= Roebling_SetPattern(rbl, wsDef, CDOC_WS, CDOC_THING);
    r |= Roebling_SetPattern(rbl, otherDef, CDOC_OTHER, CDOC_START);

    return r;
}

status Cdoc_AddParsers(MemCtx *m, Span *parsers, Lookup *desc){
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CDOC_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CDOC_THING));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)thing));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CDOC_END));

    Lookup_Add(m, desc, CDOC_START, (Abstract *)String_Make(m, bytes("START")));
    Lookup_Add(m, desc, CDOC_THING, (Abstract *)String_Make(m, bytes("THING")));
    Lookup_Add(m, desc, CDOC_END, (Abstract *)String_Make(m, bytes("END")));

    return SUCCESS;
}

Roebling *Cdoc_RblMake(MemCtx *m, FmtCtx *ctx){
    Span *parsers = Span_Make(m);
    Lookup *desc = Lookup_Make(m, CDOC_START, NULL, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    Cdoc_AddParsers(m, parsers, desc);

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        Cdoc_Capture,
        (Abstract *)ctx
    ); 
}
