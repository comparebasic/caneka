#include <external.h>
#include <caneka.h>

#include "rbl_parser_defs.c"

static word indentDef[] = {
    PAT_MANY, ' ',' ', PAT_MANY|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word sepDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, ',', ',',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word curlyCloseDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, '}', '}',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word curlyOpenDef[] = {
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE, '\n', '\n', PAT_TERM, '{', '{',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE, '\r', '\r', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word listCloseDef[] = {
    PAT_TERM, ')',')',
    PAT_END, 0, 0
};


static word lineEndDef[] = {
    PAT_ANY, ' ',' ', PAT_ANY,'\t','\t', PAT_ANY, '\r', '\r', PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word blankLineDef[] = {
    PAT_MANY, ' ',' ', PAT_MANY,'\t','\t', PAT_MANY, '\r', '\r', PAT_MANY|PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static word reqDef[] = {
    PAT_TERM, 'R', 'R',
    PAT_TERM, 'e', 'e',
    PAT_TERM, 'q', 'q',
    PAT_TERM, 'u', 'u',
    PAT_TERM, 'i', 'i',
    PAT_TERM, 'r', 'r',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word pkgDef[] = {
    PAT_TERM, 'P', 'P',
    PAT_TERM, 'a', 'a',
    PAT_TERM, 'c', 'c',
    PAT_TERM, 'k', 'k',
    PAT_TERM, 'a', 'a',
    PAT_TERM, 'g', 'g',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word structDef[] = {
    PAT_TERM, 's', 's',
    PAT_TERM, 't', 't',
    PAT_TERM, 'r', 'r',
    PAT_TERM, 'u', 'u',
    PAT_TERM, 'c', 'c',
    PAT_TERM, 't', 't',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_TERM|PAT_NO_CAPTURE, '{', '{',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE,'\t','\t', PAT_ANY|PAT_NO_CAPTURE,'\r', '\r',PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\n', '\n',
    PAT_END, 0, 0
};

static word typeDef[] = {
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'y', 'y',
    PAT_TERM, 'p', 'p',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\n', '\n',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word rblDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '/', '/',PAT_KO, '/','/', patText,
    PAT_END, 0, 0
};


static word tokenDef[] = {
    PAT_MANY|PAT_KO|PAT_LEAVE, '?', '?',PAT_MANY|PAT_KO|PAT_LEAVE, '.', '.', PAT_MANY|PAT_KO|PAT_LEAVE, '(', '(', PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', 
    PAT_END, 0, 0
};

static word invokeDef[] = {
    PAT_MANY|PAT_KO|PAT_LEAVE, '.', '.', PAT_KO, '(', '(', PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z',
/*
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '(', PAT_KO|PAT_MANY, ')', PAT_MANY|PAT_TERM, 0, 255,
    */
    PAT_END, 0, 0
};

static word toFuncDef[] = {
    PAT_MANY|PAT_TERM|PAT_NO_CAPTURE, ' ', ' ',
    PAT_TERM, '-','-',
    PAT_TERM, '>','>',
    PAT_ANY|PAT_TERM|PAT_NO_CAPTURE, ' ', ' ',
    PAT_END, 0, 0
};

static word tokenDotDef[] = {
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '.','.',
    PAT_END, 0, 0
};

static word tokenNullDef[] = {
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', PAT_TERM|PAT_NO_CAPTURE, '?','?',
    PAT_END, 0, 0
};

static word cDef[] = {
    PAT_TERM, 'C', 'C',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_TERM, '{', '{',
    PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_NO_CAPTURE|PAT_TERM,'\n', '\n',
    PAT_END, 0, 0
};

static word lineDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word opDef[] = {
    PAT_MANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_MANY, '=', '=', 
    PAT_MANY, '<', '<', 
    PAT_MANY, '>', '>', 
    PAT_MANY, 'i', 'i', 
    PAT_MANY, 's', 's', 
    PAT_MANY, 'e', 'e', 
    PAT_MANY, 'q', 'q', 
    PAT_MANY, 'c', 'c', 
    PAT_MANY, 'm', 'm', 
    PAT_MANY, 'p', 'p', 
    PAT_MANY, '!', '!', 
    PAT_MANY, '&', '&', 
    PAT_MANY, '~', '~', 
    PAT_MANY, '+', '+', 
    PAT_MANY, '-', '-', 
    PAT_MANY, '*', '*', 
    PAT_MANY|PAT_TERM, '/', '/', 
    PAT_MANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_END, 0, 0
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)blankLineDef, CNK_LANG_BLANK_LINE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)reqDef, CNK_LANG_REQUIRE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)pkgDef, CNK_LANG_PACKAGE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)typeDef, CNK_LANG_TYPE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cDef, CNK_LANG_C, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)structDef, CNK_LANG_STRUCT, CNK_LANG_STRUCT);
    return r;
}

static status str(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, CNK_LANG_STRUCT);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyCloseDef, CNK_LANG_CURLY_CLOSE, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)indentDef, CNK_LANG_INDENT, CNK_LANG_STRUCT);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNullDef, CNK_LANG_TOKEN_NULLABLE, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_TOKEN);

    return r;
}

static status token(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyCloseDef, CNK_LANG_CURLY_CLOSE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenNullDef, CNK_LANG_TOKEN_NULLABLE, CNK_LANG_POST_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_TOKEN);

    return r;
}


static status post(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)opDef, CNK_LANG_OP, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyCloseDef, CNK_LANG_CURLY_CLOSE, CNK_LANG_VALUE);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyOpenDef, CNK_LANG_CURLY_OPEN, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)curlyCloseDef, CNK_LANG_CURLY_CLOSE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_VALUE, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)invokeDef, CNK_LANG_INVOKE, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_TOKEN);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)rblDef, CNK_LANG_ROEBLING, CNK_LANG_VALUE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)toFuncDef, CNK_LANG_FUNC_PTR, CNK_LANG_VALUE);

    return r;
}

static status argList(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)listCloseDef, CNK_LANG_LIST_CLOSE, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)sepDef, 0, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, CNK_LANG_ARG_LIST);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_ARG_LIST);

    return r;
}

static status lineEnd(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineEndDef, CNK_LANG_LINE_END, -1);
    return r;
}

Roebling *CnkLangCtx_RblMake(MemCtx *m, FmtCtx *ctx, RblCaptureFunc capture){

    Span *parsers = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_STRUCT));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)str));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_TOKEN));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)token));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_VALUE));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)value));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)lineEnd));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_POST_TOKEN));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)post));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_ARG_LIST));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)argList));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_LINE_END));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)lineEnd));

    LookupConfig config[] = {
        {CNK_LANG_START, (Abstract *)String_Make(m, bytes("START"))},
        {CNK_LANG_STRUCT, (Abstract *)String_Make(m, bytes("STRUCT"))},
        {CNK_LANG_TOKEN, (Abstract *)String_Make(m, bytes("TOKEN"))},
        {CNK_LANG_VALUE, (Abstract *)String_Make(m, bytes("VALUE"))},
        {CNK_LANG_POST_TOKEN, (Abstract *)String_Make(m, bytes("POST_TOKEN"))},
        {CNK_LANG_ARG_LIST, (Abstract *)String_Make(m, bytes("ARG_LIST"))},
        {CNK_LANG_LINE_END, (Abstract *)String_Make(m, bytes("LINE_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        capture,
        (Abstract *)ctx
    ); 
}
