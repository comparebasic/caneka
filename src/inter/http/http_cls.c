#include <external.h>
#include <caneka.h>

static boolean _httpInitialized = FALSE;
Lookup *HttpMethods = NULL;

static status HttpInit(MemCh *m){
    if(!_httpInitialized){
        _httpInitialized = TRUE;
        HttpMethods = Lookup_Make(m, HTTP_METHOD);
        Lookup_Add(m, HttpMethods, HTTP_METHOD_GET, Str_CstrRef(m, "GET"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_POST, Str_CstrRef(m, "POST"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_UPDATE, Str_CstrRef(m, "UPDATE"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_DELETE, Str_CstrRef(m, "DELETE"));
        Lookup_Add(m, HttpMethods, HTTP_METHOD_PUT, Str_CstrRef(m, "PUT"));
        return SUCCESS;
    }
    return NOOP; 
}
static i64 HttpCtx_Print(Buff *bf, void *a, cls type, word flags){
    HttpCtx *ctx = (HttpCtx*)as(a, TYPE_HTTP_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        Lookup_Get(HttpMethods, ctx->method),
        ctx->path,
        ctx->headers,
        ctx->body,
        NULL,
    };
    return Fmt(bf, "Http<$ $ $ headers:@ body:@>", args);
}

status Http_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_CTX, (void *)HttpCtx_Print);
    return r;
}

status Http_ClsInit(MemCh *m){
    status r = READY;
    r |= Http_ToSInit(m, ToStreamLookup);
    r |= HttpInit(m);
    return r;
}
