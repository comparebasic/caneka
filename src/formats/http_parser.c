#include <external.h>
#include <caneka.h>

word pathDef[] = {
    PAT_TERM|PAT_NO_CAPTURE,' ',' ', 
    PAT_KO,' ',' ', 
    patText,
    PAT_END, 0, 0
};

word protoDef[] = {
    PAT_TERM,'H','H', 
    PAT_TERM,'T','T', 
    PAT_TERM,'T','T', 
    PAT_TERM,'P','P', 
    PAT_TERM,'/','/', 
    PAT_TERM,'1','1', 
    PAT_TERM,'.','.', 
    PAT_TERM,'1', '2',
    PAT_TERM|PAT_NO_CAPTURE, '\r', '\r',
    PAT_TERM|PAT_NO_CAPTURE, '\n', '\n',
    PAT_END, 0, 0
};

word headerDef[] = {
    PAT_KO, ':', ':',
    patText,
    PAT_END, 0, 0
};

word headerValueDef[] = {
    PAT_ANY|PAT_NO_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_KO, '\r', '\r', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31, 
    PAT_TERM|PAT_NO_CAPTURE|PAT_CONSUME, '\n', '\n',
    PAT_CMD|PAT_INVERT, PAT_GO_ON_FAIL, 1, PAT_INVERT|PAT_LEAVE, ' ', ' ', PAT_INVERT|PAT_LEAVE|PAT_TERM, '\t', '\t',
    PAT_END, 0, 0
};

word endDef[] = {
    PAT_TERM, '\r', '\r',
    PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static status method(Roebling *rbl){
    Lookup *lk = Lookup_Make(rbl->m, HTTP_METHOD_GET, NULL, NULL); 
    Lookup_Add(rbl->m, lk, HTTP_METHOD_GET, 
         (void *)String_Make(rbl->m, bytes("GET")));
    Lookup_Add(rbl->m, lk, HTTP_METHOD_POST, 
         (void *)String_Make(rbl->m, bytes("POST")));
    Lookup_Add(rbl->m, lk, HTTP_METHOD_UPDATE, 
         (void *)String_Make(rbl->m, bytes("UPDATE")));
    Lookup_Add(rbl->m, lk, HTTP_METHOD_SET, 
         (void *)String_Make(rbl->m, bytes("SET")));
    Lookup_Add(rbl->m, lk, HTTP_METHOD_DELETE, 
         (void *)String_Make(rbl->m, bytes("DELETE")));
 
    Roebling_ResetPatterns(rbl);
    return Roebling_SetLookup(rbl, lk, HTTP_METHOD, -1); 
}

static status path(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)pathDef, HTTP_PATH, -1);
    return r;
}

static status httpProto(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)protoDef, HTTP_PROTO, -1);
    return r;
}

static status header(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)headerDef, HTTP_HEADER, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)endDef, HTTP_END, HTTP_MARK_END);
    return r;
}

static status headerValue(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)headerValueDef, HTTP_HEADER_VALUE, HTTP_MARK_HEADER);
    return r;
}

static status httpParser_Capture(word captureKey, String *s, Abstract *source){
    if(DEBUG_HTTP){
        printf("%d\n", captureKey);
        Debug_Print(s, 0, "Captured: ", DEBUG_XML, TRUE);
        printf("\n");
    }

    status r = SUCCESS;
    return r;
}

Roebling *HttpParser_Make(MemCtx *m, String *s, Abstract *source){
    MemHandle *mh = (MemHandle *)m;
    Span *parsers_do =  Span_From(m, 7, 
            (Abstract *)Do_Wrapped(mh, (DoFunc)method),
            (Abstract *)Do_Wrapped(mh, (DoFunc)path),
            (Abstract *)Do_Wrapped(mh, (DoFunc)httpProto),
        (Abstract *)Int_Wrapped(m, HTTP_MARK_HEADER), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)header),
            (Abstract *)Do_Wrapped(mh, (DoFunc)headerValue),
        (Abstract *)Int_Wrapped(m, HTTP_MARK_END));

    LookupConfig config[] = {
        {HTTP_MARK_HEADER, (Abstract *)String_Make(m, bytes("HTTP_HEADER"))},
        {HTTP_MARK_END, (Abstract *)String_Make(m, bytes("HTTP_END"))},
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
        httpParser_Capture,
        source 
    ); 
}
