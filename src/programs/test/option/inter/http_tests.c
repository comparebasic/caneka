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

    HttpCtx *ctx = (HttpCtx*)Ifc(m, proto->ctx, TYPE_HTTP_CTX);
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
    HttpCtx *ctx = (HttpCtx*)Ifc(m, proto->ctx, TYPE_HTTP_CTX);

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
    Roebling *rbl = HttpRbl_Make(m, curs, proto);
    Roebling_Run(rbl);

    r |= Test(rbl->type.state & SUCCESS, "Roebling finished with state SUCCESS", NULL);

    NodeObj *config = Inst_Make(m, TYPE_NODEOBJ);
    HttpCtx_ParseBody(ctx, config, curs);

    args[0] = K(m, "POST");
    args[1] = Lookup_Get(HttpMethods, ctx->method);
    args[2] = NULL;
    r |= Test(ctx->method == HTTP_METHOD_POST,
        "HttpCtx method is expected @, have @", args); 

    args[0] = K(m, "/forms/signup");
    args[1] = ctx->path;
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpCtx path is expected @, have @", args); 

    args[0] = K(m, "Firefudge/Aluminum");
    args[1] = Table_Get(ctx->headersIt.p, K(m, "User-Agent"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpCtx User-Agent header is expected @, have @", args); 

    args[0] = K(m, "text/html");
    args[1] = Table_Get(ctx->headersIt.p, K(m, "Accept"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpCtx User-Agent header is expected @, have @", args); 

    Table *bodyData = (Table *)ctx->body;

    args[0] = K(m, "fancy.pantsy@example.com");
    args[1] = Table_Get(bodyData, K(m, "email"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpCtx body#email is expected @, have @", args); 

    args[0] = K(m, "Fantsy");
    args[1] = Table_Get(bodyData, K(m, "first-name"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpCtx body#first-name is expected @, have @", args); 

    DebugStack_Pop();
    return r;
}
