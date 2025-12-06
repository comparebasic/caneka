#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Http_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    ProtoCtx *proto = HttpProto_Make(m);

    StrVec *v = Sv(m, 
        "GET /fancy.html HTTP/1.1\r\n"
        "User-Agent: Firefudge/Aluminum\r\n"
        "Broken-Header: One,Two,Three,\r\n"
        "\tFour\r\n"
        "\r\n"
    );
    Cursor *curs = Cursor_Make(m, v);
    Roebling *rbl = HttpRbl_Make(m, curs, proto);
    Roebling_Run(rbl);

    HttpCtx *ctx = (HttpCtx*)as(proto->ctx, TYPE_HTTP_CTX);
    args[0] = I32_Wrapped(m, HTTP_METHOD_GET);
    args[1] = NULL;
    r |= Test(ctx->method == HTTP_METHOD_GET, "Method is as expected $", args);

    args[0] = K(m, "/fancy.html");
    args[1] = NULL;
    r |= Test(Equals(ctx->path, args[0]), "Path is as expected $", args);

    args[0] = K(m, "User-Agent");
    args[1] = K(m, "Firefudge/Aluminum");
    args[2] = NULL;
    r |= Test(Equals(Table_Get(ctx->headersIt.p, args[0]), args[1]),
        "Header is as expected for $, $", args);

    args[0] = K(m, "Broken-Header");
    args[1] = K(m, "One,Two,Three,Four");
    args[2] = NULL;
    r |= Test(Equals(Table_Get(ctx->headersIt.p, args[0]), args[1]),
        "Header is as expected for $, $", args);

    DebugStack_Pop();
    return r;
}

status HttpQuery_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    ProtoCtx *proto = HttpProto_Make(m);
    HttpCtx *ctx = (HttpCtx*)as(proto->ctx, TYPE_HTTP_CTX);

    Str *content = S(m, 
        "{\"email\": \"fancy.pantsy@example.com\", \"first-name\": \"Fantsy\"}");

    args[0] = Str_FromI64(m, content->length);
    args[1] = content;
    args[2] = NULL;
    StrVec *v = Fmt_ToStrVec(m, 
        "POST /forms/signup?action=add HTTP/1.1\r\n"
        "User-Agent: Firefudge/Aluminum\r\n"
        "Content-Type: application/json\r\n"
        "Accept: text/html\r\n"
        "Content-Length: $\r\n"
        "\r\n"
        "$", args);


    Cursor *curs = Cursor_Make(m, v);
    curs->type.state |= DEBUG;
    Roebling *rbl = HttpRbl_Make(m, curs, proto);
    Roebling_Run(rbl);

    args[0] = v;
    args[1] = curs;
    args[2] = curs->v;
    args[3] = NULL;
    Out("^c.Parsing @ curs &\n   &^0\n", args);

    r |= Test(rbl->type.state & SUCCESS, "Roebling finished with state SUCCESS", NULL);

    Single *sg = Table_Get(ctx->headersIt.p, K(m, "Content-Length"));
    StrVec *body = Cursor_Get(m, curs, sg->val.value, 1);

    args[0] = body;
    args[1] = NULL;
    Out("^c.Body &^0\n", args);

    args[0] = ctx;
    args[1] = NULL;
    Out("^p.Request Parsed @^0\n", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
