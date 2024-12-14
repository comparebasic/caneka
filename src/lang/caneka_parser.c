#include <external.h>
#include <caneka.h>

static char * cnkRangeToChars(word range){
    if(range == CNK_LANG_START){
        return "CNK_LANG_START";
    }else if(range == CNK_LANG_LINE){
        return "CNK_LANG_LINE";
    }else if(range == CNK_LANG_INDENT){
        return "CNK_LANG_INDENT";
    }else if(range == CNK_LANG_STRUCT){
        return "CNK_LANG_STRUCT";
    }else if(range == CNK_LANG_REQUIRE){
        return "CNK_LANG_REQUIRE";
    }else if(range == CNK_LANG_PACKAGE){
        return "CNK_LANG_PACKAGE";
    }else if(range == CNK_LANG_TYPE){
        return "CNK_LANG_TYPE";
    }else if(range == CNK_LANG_C){
        return "CNK_LANG_C";
    }else if(range == CNK_LANG_END_C){
        return "CNK_LANG_END_C";
    }else if(range == CNK_LANG_TOKEN){
        return "CNK_LANG_TOKEN";
    }else if(range == CNK_LANG_TOKEN_DOT){
        return "CNK_LANG_TOKEN_DOT";
    }else if(range == CNK_LANG_POST_TOKEN){
        return "CNK_LANG_POST_TOKEN";
    }else if(range == CNK_LANG_ASSIGN){
        return "CNK_LANG_ASSIGN";
    }else if(range == CNK_LANG_VALUE){
        return "CNK_LANG_VALUE";
    }else{
        return "unknown";
    }
}

static word indentDef[] = {
    PAT_MANY, ' ',' ', PAT_MANY|PAT_TERM,'\t','\t',
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
    PAT_END, 0, 0
};


static word typeDef[] = {
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'y', 'y',
    PAT_TERM, 'p', 'p',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\r', '\r',
    PAT_ANY|PAT_NO_CAPTURE, ' ',' ', PAT_ANY|PAT_NO_CAPTURE|PAT_TERM,'\t','\t',
    PAT_END, 0, 0
};

static word tokenDef[] = {
    PAT_MANY, '-', '-',PAT_MANY, '_', '_',PAT_MANY, '0', '9',PAT_MANY, 'A', 'Z',PAT_MANY|PAT_TERM, 'a', 'z', 
    PAT_END, 0, 0
};

static word tokenDotDef[] = {
    PAT_KO, '.','.',PAT_ANY, '-', '-',PAT_ANY, '_', '_',PAT_ANY, '0', '9',PAT_ANY, 'A', 'Z',PAT_ANY|PAT_TERM, 'a', 'z', 
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
    PAT_KO|PAT_CONSUME, '\n', '\n', patText,
    PAT_END, 0, 0
};

static word assignDef[] = {
    PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_TERM, '=', '=', 
    PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_END, 0, 0
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)reqDef, CNK_LANG_REQUIRE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)pkgDef, CNK_LANG_PACKAGE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)typeDef, CNK_LANG_TYPE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cDef, CNK_LANG_C, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)structDef, CNK_LANG_STRUCT, CNK_LANG_STRUCT);

    PatCharDef *def = (PatCharDef *)lineDef;
    Match *mt = Span_ReserveNext(rbl->matches);
    r |= Match_SetPattern(mt, def);
    mt->captureKey = CNK_LANG_LINE;
    mt->jump = Roebling_GetMarkIdx(rbl, CNK_LANG_START);
    mt->type.state |= SUCCESS_EMPTY;

    return r;
}

static status str(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)indentDef, CNK_LANG_INDENT, CNK_LANG_STRUCT);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_PACKAGE, CNK_LANG_STRUCT);

    return r;
}

static status post(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)assignDef, CNK_LANG_ASSIGN, CNK_LANG_VALUE);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDef, CNK_LANG_TOKEN, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tokenDotDef, CNK_LANG_TOKEN_DOT, CNK_LANG_VALUE);

    return r;
}


static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    CnkLangCtx *ctx = (CnkLangCtx *)as(source, TYPE_LANG_CNK);
    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture: %s", DEBUG_LANG_CNK, cnkRangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }
    return SUCCESS;
}

CnkLangSpace *CnkLangSpace_Make(MemCtx *m){
    CnkLangSpace *space = MemCtx_Alloc(m, sizeof(CnkLangSpace));
    space->type.of = TYPE_LANG_CNK_SPACE;
    return space;
}

CnkLangCtx *CnkLangCtx_Make(MemCtx *m){
    CnkLangCtx *ctx = MemCtx_Alloc(m, sizeof(CnkLangCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;


    Span *parsers = Span_Make(m, TYPE_SPAN);
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_START));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_STRUCT));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)str));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_POST_TOKEN));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)post));
    Span_Add(parsers, (Abstract *)Int_Wrapped(m, CNK_LANG_VALUE));
    Span_Add(parsers, (Abstract *)Do_Wrapped(m, (DoFunc)value));

    LookupConfig config[] = {
        {CNK_LANG_START, (Abstract *)String_Make(m, bytes("START"))},
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
    ctx->space = CnkLangSpace_Make(m);

    return ctx;
}
