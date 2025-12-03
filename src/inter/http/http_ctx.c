#include <external.h>
#include <caneka.h>

static Lookup *statusCodeStrings = NULL;

status HttpCtx_WriteHeaders(Buff *bf, HttpCtx *ctx){
    status r = READY;
    Str *status = Lookup_Get(statusCodeStrings, ctx->code);
    if(status == NULL){
        status = Lookup_Get(statusCodeStrings, 500);
        r |= ERROR;
    }

    void *args[] = {
        status,
        ctx->mime,
        I64_Wrapped(bf->m, ctx->contentLength),
        NULL
    };

    r |= Fmt(bf, "HTTP/1.1 $\r\n"
        "Server: Caneka\r\n"
        "Content-Type: $\r\n"
        "Content-Length: $\r\n"
        "\r\n" , args);
    return r;
}

HttpCtx *HttpCtx_Make(MemCh *m){
    HttpCtx *ctx = (HttpCtx *)MemCh_AllocOf(m, sizeof(HttpCtx), TYPE_HTTP_CTX);
    ctx->type.of = TYPE_HTTP_CTX;
    ctx->path = StrVec_Make(m);
    ctx->mime = S(m, "text/plain");
    ctx->data = Table_Make(m);
    Iter_Init(&ctx->headersIt, Table_Make(m));
    return ctx;
}

status HttpCtx_Init(MemCh *m){
    status r = READY;
    if(statusCodeStrings == NULL){
        statusCodeStrings = Lookup_Make(m, 200);
        r |= Lookup_Add(m,
            statusCodeStrings, 200, Str_FromCstr(m, "200 Ok", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 301, Str_FromCstr(m, "301 Redirect", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 302, Str_FromCstr(m, "301 Temporary Redirect", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 404, Str_FromCstr(m, "404 Not Found", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 403, Str_FromCstr(m, "403 Forbidden", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 500, Str_FromCstr(m, "500 Server Error", STRING_COPY));
    }
    return r;
}
