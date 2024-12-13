#include <external.h>
#include <caneka.h>

static char * cnkRangeToChars(word range){
    if(range == CNK_LANG_START){
        return "CNK_LANG_START";
    }else if(range == CNK_LANG_LINE){
        return "CNK_LANG_LINE";
    }else if(range == CNK_LANG_TYPE){
        return "CNK_LANG_TYPE";
    }else if(range == CNK_LANG_C){
        return "CNK_LANG_C";
    }else if(range == CNK_LANG_END_C){
        return "CNK_LANG_END_C";
    }else if(range == CNK_LANG_VARDEF){
        return "CNK_LANG_VARDEF";
    }else if(range == CNK_LANG_ASSIGN_VAL){
        return "CNK_LANG_ASSIGN_VAL";
    }else{
        return "unknown";
    }
}

static word typeDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '#', '#',
    PAT_TERM, 'T', 'T',
    PAT_TERM, 'y', 'y',
    PAT_TERM, 'p', 'p',
    PAT_TERM, 'e', 'e',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\r', '\r',
    PAT_END, 0, 0
};

static word cDef[] = {
    PAT_KO, '\n', '\n',
    PAT_TERM|PAT_NO_CAPTURE, '#', '#',
    PAT_TERM, 'C', 'C',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\r', '\r',
    PAT_END, 0, 0
};

static word cEndDef[] = {
    PAT_KO, '\n', '\n',
    PAT_TERM|PAT_NO_CAPTURE, '#', '#',
    PAT_TERM, 'e', 'e',
    PAT_TERM, 'n', 'n',
    PAT_TERM, 'd', 'd',
    PAT_TERM, 'C', 'C',
    PAT_MANY|PAT_NO_CAPTURE, ' ',' ', PAT_MANY|PAT_NO_CAPTURE,'\t','\t', PAT_MANY|PAT_NO_CAPTURE,'\r', '\r',PAT_MANY|PAT_NO_CAPTURE|PAT_TERM,'\r', '\r',
    PAT_END, 0, 0
};

static word lineDef[] = {
    PAT_KO, '\n', '\n', patText,
    PAT_END, 0, 0
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)typeDef, CNK_LANG_TYPE, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cDef, CNK_LANG_C, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)cEndDef, CNK_LANG_END_C, CNK_LANG_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)lineDef, CNK_LANG_LINE, CNK_LANG_START);

    return r;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    CnkLangCtx *ctx = (CnkLangCtx *)as(source, TYPE_LANG_CNK);
    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture: %s - '%s'\x1b[0m\n", DEBUG_LANG_CNK, cnkRangeToChars(captureKey), s->bytes);
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
