#include <external.h>
#include <caneka.h>

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

    HttpCtx *ctx = (HttpCtx*)as(proto->data, TYPE_HTTP_CTX);
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
    
    microTime time = MicroTime_Now();
    StrVec *ua = Table_Get(ctx->headersIt.p, K(m, "User-Agent"));
    StrVec *ssid = Ssid_From(m, ua, time);
    args[0] = ssid;
    args[1] = NULL;
    r |= Test(ssid->total == 42, "Ssid has length of 34, have @", args);
    quad parity = Parity_FromVec(ua);
    args[0] = Str_ToHex(m, Str_Ref(m, (byte *)&parity, sizeof(quad), sizeof(quad), STRING_BINARY));
    args[1] = Span_Get(ssid->p, 0);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "Ssid first seg is parity of User-Agent, expected @, have @", args);
    args[0] = Str_ToHex(m, Str_Ref(m, (byte *)&time, sizeof(microTime), sizeof(microTime), STRING_BINARY));
    args[1] = Span_Get(ssid->p, 2);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "Ssid second seg is time, expected @, have @", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
