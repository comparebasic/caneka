#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Http_Tests(MemCh *m){
    Debug_Push(m, NULL);

    void *args[5];
    status r = READY;

    IoCtx *ctx = IoCtx_Make(m);
    HttpReq *req = (HttpReq *)HttpReq_Mk(ctx);

    StrVec *v = Sv(m, 
        "GET /fancy.html HTTP/1.1\r\n"
        "User-Agent: Firefudge/Aluminum\r\n"
        "Broken-Header: One,Two,Three,\r\n"
        "\tFour\r\n"
        "\r\n"
    );
    Cursor *curs = Cursor_Make(m, v);
    Roebling *rbl = HttpRbl_Make(m, curs, req);
    Roebling_Run(rbl);

    args[0] = HttpMethodStr(m, METHOD_HTTP_GET);
    args[1] = HttpMethodStr(m, req->address.method);
    args[1] = NULL;
    r |= Test(req->address.method == METHOD_HTTP_GET, "Method is as expected $, have $", args);

    args[0] = K(m, "/fancy.html");
    args[1] = NULL;
    r |= Test(Equals(req->path, args[0]), "Path is as expected $", args);

    args[0] = K(m, "User-Agent");
    args[1] = K(m, "Firefudge/Aluminum");
    args[2] = NULL;
    r |= Test(Equals(Table_Get(req->headersIt.p, args[0]), args[1]),
        "Header is as expected for $, $", args);

    args[0] = K(m, "Broken-Header");
    args[1] = K(m, "One,Two,Three,Four");
    args[2] = NULL;
    r |= Test(Equals(Table_Get(req->headersIt.p, args[0]), args[1]),
        "Header is as expected for $, $", args);

    Return(m, r);
}

status HttpQuery_Tests(MemCh *m){
    Debug_Push(m, NULL);

    void *args[5];
    status r = READY;

    IoCtx *ctx = IoCtx_Make(m);
    HttpReq *req = (HttpReq *)HttpReq_Mk(ctx);

    Str *content = S(m, 
        "{\"email\": \"fancy.pantsy@example.com\", \"first-name\": \"Fantsy\"}");

    args[0] = Str_FromI64(m, content->length);
    args[1] = content;
    args[2] = NULL;
    Fmt(req->in, 
        "POST /forms/signup?action=add HTTP/1.1\r\n"
        "User-Agent: Firefudge/Aluminum\r\n"
        "Content-Type: application/json\r\n"
        "Accept: text/html\r\n"
        "Content-Length: $\r\n"
        "\r\n"
        "$", args);
    
    Roebling_Run(req->rbl);

    args[0] = req->in->v;
    args[1] = req->rbl->curs;
    args[2] = NULL;
    Out("^y.Request body:\n^c.$^0\nread: ^y.&^0\n", args);

    r |= Test(req->rbl->type.state & SUCCESS, "Roebling finished with state SUCCESS", NULL);

    Node *config = Inst_Make(m, TYPE_NODE);
    Buff *bf = Buff_Make(m, ZERO);
    HttpReq_ParseBody(req);

    args[0] = K(m, "POST");
    args[0] = NULL;
    r |= Test(req->address.method == METHOD_HTTP_POST,
        "HttpReq method is expected @", args); 

    args[0] = K(m, "/forms/signup");
    args[1] = req->path;
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpReq path is expected @, have @", args); 

    args[0] = K(m, "Firefudge/Aluminum");
    args[1] = Table_Get(req->headersIt.p, K(m, "User-Agent"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpReq User-Agent header is expected @, have @", args); 

    args[0] = K(m, "text/html");
    args[1] = Table_Get(req->headersIt.p, K(m, "Accept"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpReq User-Agent header is expected @, have @", args); 

    Table *bodyData = (Table *)req->body;

    args[0] = K(m, "fancy.pantsy@example.com");
    args[1] = Table_Get(bodyData, K(m, "email"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpReq body#email is expected @, have @", args); 

    args[0] = K(m, "Fantsy");
    args[1] = Table_Get(bodyData, K(m, "first-name"));
    args[2] = NULL;
    r |= Test(Equals(args[0],args[1]), 
        "HttpReq body#first-name is expected @, have @", args); 

    Return(m, r);
}
