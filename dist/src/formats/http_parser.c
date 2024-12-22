#include <external.h>
#include <caneka.h>

char *HttpProto_MethodToChars(int methodIdx){
    if(methodIdx == HTTP_METHOD_GET){
        return "GET";
    }else if(methodIdx == HTTP_METHOD_GET){
        return "POST";
    }else if(methodIdx == HTTP_METHOD_GET){
        return "UPDATE";
    }else if(methodIdx == HTTP_METHOD_GET){
        return "SET";
    }else if(methodIdx == HTTP_METHOD_GET){
        return "DELETE";
    }
    return "UNKNOWN_method";
}


word pathDef[] = {
    PAT_TERM|PAT_INVERT_CAPTURE,' ',' ', 
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
    PAT_TERM|PAT_INVERT_CAPTURE, '\r', '\r',
    PAT_TERM|PAT_INVERT_CAPTURE, '\n', '\n',
    PAT_END, 0, 0
};

word headerDef[] = {
    PAT_KO, ':', ':',
    patText,
    PAT_END, 0, 0
};

word headerValueDef[] = {
    PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' ', 
    PAT_KO, '\r', '\r', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31, 
    PAT_TERM|PAT_INVERT_CAPTURE, '\n', '\n',
    PAT_CMD|PAT_INVERT, PAT_GO_ON_FAIL, 1, PAT_INVERT|PAT_LEAVE, ' ', ' ', PAT_INVERT|PAT_LEAVE|PAT_TERM, '\t', '\t',
    PAT_END, 0, 0
};

word endDef[] = {
    PAT_TERM, '\r', '\r',
    PAT_TERM, '\n', '\n',
    PAT_END, 0, 0
};

static status method(MemCtx *m, Roebling *rbl){
    /*
    HttpProto *proto = (HttpProto *)rbl->source;
    Roebling_ResetPatterns(rbl);
    return Roebling_SetLookup(rbl, proto->methods, HTTP_METHOD, -1); 
    */
    return NOOP;
}

static status path(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)pathDef, HTTP_PATH, -1);
    return r;
}

static status httpProto(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)protoDef, HTTP_PROTO, -1);
    return r;
}

static status header(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)headerDef, HTTP_HEADER, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)endDef, HTTP_END, HTTP_MARK_END);
    return r;
}

static status headerValue(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)headerValueDef, HTTP_HEADER_VALUE, HTTP_MARK_HEADER);
    return r;
}

static status httpParser_Capture(word captureKey, int matchIdx, String *s, Abstract *source){

    if(DEBUG_HTTP){
        printf("%d\n", captureKey);
        Debug_Print(s, 0, "Captured: ", DEBUG_HTTP, TRUE);
        printf("\n");
    }

    status r = SUCCESS;
    return r;
}

Roebling *HttpParser_Make(MemCtx *m, String *s, Abstract *source){
    Span *parsers_do =  Span_From(m, 7, 
            (Abstract *)Do_Wrapped(m, (DoFunc)method),
            (Abstract *)Do_Wrapped(m, (DoFunc)path),
            (Abstract *)Do_Wrapped(m, (DoFunc)httpProto),
        (Abstract *)Int_Wrapped(m, HTTP_MARK_HEADER), 
            (Abstract *)Do_Wrapped(m, (DoFunc)header),
            (Abstract *)Do_Wrapped(m, (DoFunc)headerValue),
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
