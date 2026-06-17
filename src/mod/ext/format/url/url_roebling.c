#include <external.h>
#include <caneka.h>

static PatCharDef protoDef[] = {
    {PAT_KO|PAT_KO_TERM, ':', ':'}, {PAT_KO|PAT_KO_TERM, '/', '/'}, {PAT_KO|PAT_KO_TERM, '/', '/'},
    {PAT_MANY,'a' ,'z'},
    {PAT_MANY|PAT_TERM,'A' ,'Z'},
    {PAT_END, 0, 0}
};

static PatCharDef domainSegDef[] = {
    {PAT_MANY,'0' ,'9'},
    {PAT_MANY,'a' ,'z'},
    {PAT_MANY,'A' ,'Z'},
    {PAT_MANY,'-' ,'-'},
    {PAT_MANY|PAT_TERM,'_' ,'_'},
    {PAT_END, 0, 0}
};

static PatCharDef dotDef[] = {
    {PAT_TERM,'.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef slashDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE,'/' ,'/'},
    {PAT_END, 0, 0}
};

static PatCharDef pathDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '?', '?'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef queryStartDef[] = {
    {PAT_TERM, '?', '?'},
    {PAT_END, 0, 0}
};

static PatCharDef queryNextDef[] = {
    {PAT_TERM, '&', '&'},
    {PAT_END, 0, 0}
};

static PatCharDef queryNextValueDef[] = {
    {PAT_TERM, '=', '='},
    {PAT_END, 0, 0}
};

static PatCharDef queryEscapedDef[] = {
    {PAT_SINGLE|PAT_TERM, '%', '%'},
    {PAT_SINGLE, 'A', 'F'},{PAT_SINGLE, 'a', 'f'},{PAT_SINGLE|PAT_TERM, '0', '9'},
    {PAT_SINGLE, 'A', 'F'},{PAT_SINGLE, 'a', 'f'},{PAT_SINGLE|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static PatCharDef queryKeySegDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_KO|PAT_INVERT_CAPTURE, '%', '%'},
    {PAT_KO|PAT_INVERT_CAPTURE, '=', '='},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '&', '&'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef queryValueSegDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_KO|PAT_INVERT_CAPTURE, '=', '='},
    {PAT_KO|PAT_INVERT_CAPTURE, '&', '&'},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '%', '%'},
    patText,
    {PAT_END, 0, 0}
};

static status proto(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        protoDef, URI_PROTO, URI_DOMAIN);

    return r;
}

static status domain(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        slashDef, URI_PATH_BEGIN, URI_PATH);
    r |= Roebling_SetPattern(rbl,
        queryStartDef, HTTP_QUERY_START, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        dotDef, URI_DOMAIN_SEP, URI_DOMAIN);
    r |= Roebling_SetPattern(rbl,
        domainSegDef, URI_DOMAIN_SEG, URI_DOMAIN);

    return r;
}

static status path(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        queryStartDef, HTTP_QUERY_START, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        pathDef, URI_PATH, URI_PATH);

    return r;
}

static status query(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        queryNextDef, HTTP_QUERY_NEXT, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        queryNextValueDef, HTTP_QUERY_NEXT_VALUE, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        queryEscapedDef, HTTP_QUERY_SEG_ESCAPED, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        queryKeySegDef, HTTP_QUERY_SEG_KEY, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        queryValueSegDef, HTTP_QUERY_SEG_VALUE, HTTP_QUERY);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;
    void *args[3];
    Uri *uri = (Uri *)rbl->source;
    if(rbl->curs->type.state & DEBUG){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v;
        args[2] = NULL;
        Out("^y.Token: $/@^0\n", args);
    }
    if(captureKey == URI_PROTO){
        uri->proto = Ifc(m, v, TYPE_STR);
    }else if(captureKey == URI_DOMAIN_SEG){
        StrVec_AddVec(uri->host, v);
    }else if(captureKey == URI_DOMAIN_SEP){
        StrVec_Add(uri->host, Str_Ref(m, (byte *)".", 1, 2, STRING_COPY|MORE));
    }else if(captureKey == URI_PATH){
        uri->path = v;
    }else if(captureKey == HTTP_QUERY_START){
        if(rbl->shelf == NULL){
            rbl->shelf = StrVec_Make(m);
        }
    }else if(captureKey == HTTP_QUERY_NEXT){
        if(rbl->shelf != NULL && rbl->shelf->total > 0){
            Table_SetValue(&uri->queryIt, rbl->shelf);
            rbl->shelf = StrVec_Make(m);
        }
    }else if(captureKey == HTTP_QUERY_NEXT_VALUE || captureKey == HTTP_QUERY_END){
        if(rbl->shelf != NULL && rbl->shelf->total){
            if(uri->queryIt.metrics.selected == -1){
                Table_SetKey(&uri->queryIt, rbl->shelf);
                rbl->shelf = StrVec_Make(m);
            }else{
                Table_SetValue(&uri->queryIt, rbl->shelf);
                rbl->shelf = StrVec_Make(m);
            }
        }
    }else if(captureKey == HTTP_QUERY_SEG_ESCAPED){
        Str *s = StrVec_Str(m, v);
        Str_Incr(s, 1);
        StrVec_Add(rbl->shelf, Str_FromHexFiltered(m, s));
    }else if(captureKey == HTTP_QUERY_SEG_KEY || captureKey == HTTP_QUERY_SEG_VALUE){
        StrVec_AddVec(rbl->shelf, v);
    }

    return SUCCESS;
}

Roebling *UriRbl_Make(MemCh *m, Cursor *curs, void *source){
    Debug_Push(m, curs);

    Roebling *rbl = Roebling_Make(m, curs, Capture, source); 
    Roebling_AddStep(rbl, I16_Wrapped(m, URI_PROTO));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)proto));
    Roebling_AddStep(rbl, I16_Wrapped(m, URI_DOMAIN));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)domain));
    Roebling_AddStep(rbl, I16_Wrapped(m, URI_PATH));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)path));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_QUERY));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)query));
    Roebling_AddStep(rbl, I16_Wrapped(m, URI_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;

    Return(m, rbl);
}
