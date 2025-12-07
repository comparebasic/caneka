#include <external.h>
#include <caneka.h>

static Lookup *statusCodeStrings = NULL;

StrVec *HttpCtx_MakeEtag(MemCh *m, Str *path, struct timespec *mod){
    StrVec *v = StrVec_Make(m);
    quad hpar = HalfParity_From(path);
    Str s = {.type = {TYPE_STR, STRING_BINARY|STRING_CONST}, 
        .length = sizeof(quad),
        .alloc = sizeof(quad),
        .bytes = (byte *)&hpar
    };

    StrVec_Add(v, Str_ToHex(m, &s));
    StrVec_Add(v, Str_Ref(m, (byte *)"-", 1, 1, STRING_COPY|MORE));
    StrVec_Add(v, Str_FromI64(m, mod->tv_sec));
    return v;
}

status HttpCtx_ParseBody(HttpCtx *ctx, Cursor *curs){
    MemCh *m = ctx->m;
    Single *sg = Table_Get(ctx->headersIt.p, K(m, "Content-Length"));
    if(sg != NULL){
        StrVec *contentType = Table_Get(ctx->headersIt.p, K(m, "Content-Type"));
        if(contentType != NULL && Equals(contentType, K(m, "application/json"))){
            if(curs->v->total - (Cursor_Pos(curs)+1) != sg->val.value){
                ctx->type.state |= ERROR;
                return ctx->type.state;
            }
            /* TODO: handle patterns that end with ANY without this hack */
            Cursor_Add(curs, S(m, "\0"));

            NodeObj *node = Inst_Make(m, TYPE_NODEOBJ);
            Roebling *rbl = JsonParser_Make(m, curs, node);
            rbl->dest->type.state |= DEBUG;
            Roebling_Run(rbl);
            if(rbl->type.state & SUCCESS){
                ctx->body = (Abstract *)node;
            }
        }
    }

    return ctx->type.state;
}

status HttpCtx_WriteHeaders(Buff *bf, HttpCtx *ctx){
    status r = READY;
    void *args[6];
    Str *status = Lookup_Get(statusCodeStrings, ctx->code);
    if(status == NULL){
        status = Lookup_Get(statusCodeStrings, 500);
        r |= ERROR;
    }

    struct timespec now;
    Time_Now(&now);

    args[0] = status;
    args[1] = Time_ToRStr(bf->m, &now);
    args[2] = ctx->mime;
    args[3] = NULL;
    r |= Fmt(bf, "HTTP/1.1 $\r\n"
        "Date: $\r\n"
        "Server: caneka/0.1\r\n"
        "Content-Type: $\r\n"
        "\r\n" , args);

    Iter it;
    Iter_Init(&it, ctx->headersOut);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);  
        if(h == NULL){
            ToS(bf, h->key, 0, ZERO);
            Buff_AddBytes(bf, (byte *)": ", 2);
            ToS(bf, h->value, 0, ZERO);
            Buff_AddBytes(bf, (byte *)"\r\n", 2);
        }
    }

    if(ctx->code == 304){
        return r;
    }

    args[0] = I64_Wrapped(bf->m, ctx->contentLength),
    args[1] = NULL;
    r |= Fmt(bf, "Content-Length: $\r\n", args);

    return r;
}

HttpCtx *HttpCtx_Make(MemCh *m){
    HttpCtx *ctx = (HttpCtx *)MemCh_AllocOf(m, sizeof(HttpCtx), TYPE_HTTP_CTX);
    ctx->type.of = TYPE_HTTP_CTX;
    ctx->m = m;
    ctx->path = StrVec_Make(m);
    ctx->mime = S(m, "text/plain");
    ctx->data = Table_Make(m);
    ctx->headersOut = Table_Make(m);
    Iter_Init(&ctx->headersIt, Table_Make(m));
    Iter_Init(&ctx->queryIt, Table_Make(m));
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
            statusCodeStrings, 304, Str_FromCstr(m, "304 Not Modified", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 404, Str_FromCstr(m, "404 Not Found", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 403, Str_FromCstr(m, "403 Forbidden", STRING_COPY));
        r |= Lookup_Add(m,
            statusCodeStrings, 500, Str_FromCstr(m, "500 Server Error", STRING_COPY));
    }
    return r;
}
