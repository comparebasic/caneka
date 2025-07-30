#include <external.h>
#include <caneka.h>

static PatCharDef textDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,'$' ,'$'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varDef[] = {
    {PAT_TERM, '$' ,'$'},
    {PAT_INVERT|PAT_INVERT_CAPTURE, '_', '_'},
        {PAT_INVERT|PAT_INVERT_CAPTURE, '@', '@'},
        {PAT_TERM|PAT_INVERT|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_END, 0, 0}
};

static PatCharDef varKeyValueDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM, '@' ,'@'},
    {PAT_END, 0, 0}
};

static PatCharDef varNameValueDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM, '_' ,'_'},
    {PAT_END, 0, 0}
};

static PatCharDef varKeyDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_KO|PAT_KO_TERM, '}', '}'},
    {PAT_DROPOUT, ':', ':'},{PAT_DROPOUT, '/', '/'},{PAT_DROPOUT, '?', '?'},
        {PAT_DROPOUT, '!', '!'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varEndIfDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM, '/' ,'/'},
    {PAT_TERM, '?' ,'?'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef varIfDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '?' ,'?'},
    {PAT_KO|PAT_KO_TERM, '}', '}'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varEndForDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '/' ,'/'},
    {PAT_TERM, ':' ,':'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef varForDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM|PAT_INVERT_CAPTURE, ':' ,':'},
    {PAT_KO|PAT_KO_TERM, '}', '}'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varEndIfNotDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM, '/' ,'/'},
    {PAT_TERM, '!' ,'!'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '}', '}'},
    {PAT_END, 0, 0}
};


static PatCharDef varIfNotDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '!' ,'!'},
    {PAT_KO|PAT_KO_TERM, '}', '}'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varIdxDef[] = {
    {PAT_TERM, '$' ,'$'},{PAT_TERM|PAT_INVERT_CAPTURE, '[', '['},
    {PAT_KO|PAT_KO_TERM, ']', ']'},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static status text(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        textDef, FORMAT_CASH_TEXT, FORMAT_CASH_VAR);

    return r;
}

static status var(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        varDef, FORMAT_CASH_VAR, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varKeyValueDef, FORMAT_CASH_VAR_KEYVALUE, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varNameValueDef, FORMAT_CASH_VAR_NAMEVALUE, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varEndIfDef, FORMAT_CASH_VAR_ENDIF, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varEndIfNotDef, FORMAT_CASH_VAR_ENDIFNOT, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varEndForDef, FORMAT_CASH_VAR_ENDFOR, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varIfDef, FORMAT_CASH_VAR_ENDFOR, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varIfNotDef, FORMAT_CASH_VAR_IFNOT, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varForDef, FORMAT_CASH_VAR_FOR, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varKeyDef, FORMAT_CASH_VAR_KEY, FORMAT_CASH_TEXT);
    r |= Roebling_SetPattern(rbl,
        varIdxDef, FORMAT_CASH_VAR_IDX, FORMAT_CASH_TEXT);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    Mess *mess = rbl->mess;
    TemplCtx *ctx = (TemplCtx*)as(rbl->source, TYPE_TEMPL_CTX);
    if(rbl->mess->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey),
            (Abstract *)v,
            NULL
        };
        Out("^c. Templ Capture ^E0.$^ec./@^0.\n", args);
    }
    if(captureKey == FORMAT_CASH_TEXT){
        Iter_Add(&ctx->it, v);
    }else{
        Iter_Add(&ctx->it, Ptr_Wrapped(rbl->m, v, captureKey));
    }
    return SUCCESS;
}

Roebling *Templ_RoeblingMake(MemCh *m, Cursor *curs, Abstract *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_CASH_TEXT));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_CASH_VAR));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)var));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = Lookup_Make(m, _APPS_CASH_START, NULL, NULL);
    rbl->source = source;
    return rbl;
}
