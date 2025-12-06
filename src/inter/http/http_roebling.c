#include <external.h>
#include <caneka.h>

static PatCharDef getDef[] = {
    {PAT_TERM,'G' ,'G'},
    {PAT_TERM,'E' ,'E'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM|PAT_MANY|PAT_INVERT_CAPTURE|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef postDef[] = {
    {PAT_TERM,'P' ,'P'},
    {PAT_TERM,'O' ,'O'},
    {PAT_TERM,'S' ,'S'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM|PAT_MANY|PAT_INVERT_CAPTURE|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef uriDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, '?', '?'},{PAT_KO|PAT_KO_TERM, ' ', ' '},
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

static PatCharDef queryEndDef[] = {
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_END, 0, 0}
};

static PatCharDef versionDef[] = {
    {PAT_TERM,'H' ,'H'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM,'P' ,'P'},
    {PAT_TERM,'/' ,'/'},
    {PAT_TERM,'1' ,'1'},
    {PAT_TERM,'.' ,'.'},
    {PAT_TERM,'1' ,'2'},
    {PAT_END, 0, 0}
};

static PatCharDef endlDef[] = {
    {PAT_TERM, '\r', '\r'},
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef headerNameDef[] = {
    {PAT_SINGLE, 'A', 'Z'},{PAT_SINGLE|PAT_TERM, 'a', 'z'},
    {PAT_KO|PAT_KO_TERM, ':', ':'},
    {PAT_MANY, 'A', 'Z'},{PAT_MANY, 'a', 'z'},{PAT_MANY, '0', '9'},
    {PAT_MANY|PAT_TERM, '-', '-'},
    {PAT_END, 0, 0}
};

static PatCharDef headerValueDef[] = {
    {PAT_KO|PAT_KO_TERM, '\r', '\r'},
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    {PAT_MANY|PAT_INVERT_CAPTURE, ' ', ' '},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef headerIntValueDef[] = {
    {PAT_KO|PAT_KO_TERM, '\r', '\r'},
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    {PAT_MANY|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static PatCharDef headerContinueDef[] = {
    {PAT_MANY|PAT_INVERT_CAPTURE, '\t', '\t'},{PAT_MANY|PAT_INVERT_CAPTURE, ' ', ' '},
    {PAT_KO|PAT_KO_TERM, '\r', '\r'},
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    patText,
    {PAT_END, 0, 0}
};

static status method(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        getDef, HTTP_METHOD_GET, HTTP_PATH);
    r |= Roebling_SetPattern(rbl,
        postDef, HTTP_METHOD_POST, HTTP_PATH);

    return r;
}

static status uri(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        uriDef, HTTP_PATH, HTTP_VERSION);

    return r;
}

static status version(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
       queryStartDef, HTTP_QUERY_START, HTTP_QUERY);
    r |= Roebling_SetPattern(rbl,
        versionDef, HTTP_VERSION, HTTP_PROTO_END);
    return r;
}

static status query(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        queryEndDef, HTTP_QUERY_END, HTTP_VERSION);
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

static status protoNl(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        endlDef, HTTP_PROTO_END, HTTP_HEADER_NAME);
    return r;
}

static status headerName(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        headerNameDef, HTTP_HEADER_NAME, HTTP_HEADER_VALUE);
    r |= Roebling_SetPattern(rbl,
        headerContinueDef, HTTP_HEADER_CONTINUED, HTTP_HEADER_VALUE);
    r |= Roebling_SetPattern(rbl,
       endlDef , HTTP_END, HTTP_END);

    return r;
}

static status headerValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        headerIntValueDef, HTTP_HEADER_INT_VALUE, HTTP_HEADER_NAME);
    r |= Roebling_SetPattern(rbl,
        headerValueDef, HTTP_HEADER_VALUE, HTTP_HEADER_NAME);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;
    void *args[3];
    ProtoCtx *proto = (ProtoCtx *)as(rbl->source, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->ctx, TYPE_HTTP_CTX);
    if(rbl->curs->type.state & DEBUG){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v;
        args[2] = NULL;
        Out("^y.Token: $/@^0\n", args);
    }
    if(captureKey == HTTP_METHOD_GET){
        ctx->method = HTTP_METHOD_GET; 
    }else if(captureKey == HTTP_PATH){
        ctx->path = v;
    }else if(captureKey == HTTP_VERSION){
        ctx->httpVersion = v;
    }else if(captureKey == HTTP_HEADER_NAME){
        Table_SetKey(&ctx->headersIt, v);
    }else if(captureKey == HTTP_HEADER_VALUE){
        i32 selected = ctx->headersIt.metrics.selected;
        Table_SetValue(&ctx->headersIt, v);
        ctx->headersIt.metrics.selected = selected;
    }else if(captureKey == HTTP_HEADER_INT_VALUE){
        i32 selected = ctx->headersIt.metrics.selected;
        Table_SetValue(&ctx->headersIt, I64_Wrapped(m, I64_FromStr(StrVec_Str(m, v))));
        ctx->headersIt.metrics.selected = selected;
    }else if(captureKey == HTTP_QUERY_START){
        if(rbl->shelf == NULL){
            rbl->shelf = StrVec_Make(m);
        }
    }else if(captureKey == HTTP_QUERY_NEXT){
        if(rbl->shelf != NULL && rbl->shelf->total > 0){
            Table_SetValue(&ctx->queryIt, rbl->shelf);
            rbl->shelf = StrVec_Make(m);
        }
    }else if(captureKey == HTTP_QUERY_NEXT_VALUE || captureKey == HTTP_QUERY_END){
        if(rbl->shelf != NULL && rbl->shelf->total){
            if(ctx->queryIt.metrics.selected == -1){
                Table_SetKey(&ctx->queryIt, rbl->shelf);
                rbl->shelf = StrVec_Make(m);
            }else{
                Table_SetValue(&ctx->queryIt, rbl->shelf);
                rbl->shelf = StrVec_Make(m);
            }
        }
    }else if(captureKey == HTTP_QUERY_SEG_ESCAPED){
        Str *s = StrVec_Str(m, v);
        Str_Incr(s, 1);
        StrVec_Add(rbl->shelf, Str_FromHexFiltered(m, s));
    }else if(captureKey == HTTP_QUERY_SEG_KEY || captureKey == HTTP_QUERY_SEG_VALUE){
        StrVec_AddVec(rbl->shelf, v);
    }else if(captureKey == HTTP_HEADER_CONTINUED){
        if(ctx->headersIt.metrics.selected >= 0){
            Hashed *h = Span_Get(ctx->headersIt.p, ctx->headersIt.metrics.selected);
            StrVec *hdr = h->value;
            StrVec_AddVec(hdr, v);
        }
    }
    return SUCCESS;
}

Roebling *HttpRbl_Make(MemCh *m, Cursor *curs, void *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, source); 
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_METHOD));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)method));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_PATH));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)uri));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_QUERY));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)query));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_VERSION));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)version));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_PROTO_END));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)protoNl));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_HEADER_NAME));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)headerName));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_HEADER_VALUE));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)headerValue));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTTP_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;
    DebugStack_Pop();
    return rbl;
}
