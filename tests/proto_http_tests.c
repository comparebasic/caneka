#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status ProtoHttp_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    HttpProto_Init(m);

    Req *req = NULL;
    HttpProto *proto = NULL;


    Serve *sctx = Serve_Make(m, HttpProtoDef_Make(m));

    req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    proto = (HttpProto *) as(req->proto, TYPE_HTTP_PROTO);
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("GET /page1.html HTTP/1.2\r\nUser-Agent: Firefox\r\nCookie: sid=xyz1234;Expiration=2024-04-04;Secure=true;\r\nAccept: text/html,application/json\r\n\r\n")));

    Roebling_Run(req->in.rbl);

    printf("\n");
    Debug_Print((void *)req, 0, "Req in test", COLOR_DARK, FALSE);
    printf("\n");
    Debug_Print((void *)req->in.shelf, 0, "Req for get request: ", COLOR_DARK, TRUE);
    printf("\n");

    String *method_s = Lookup_Get(sctx->def->methods, proto->method);
    r |= Test(proto->method == TYPE_METHOD_GET, "Expect method to be set to get found %d(%s)", proto->method, method_s->bytes);
    r |= Test(String_Equals(proto->path, String_Make(m, bytes("/page1.html"))), "Expect string path to equal have '%s'", proto->path->bytes);

    req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    proto = (HttpProto *) as(req->proto, TYPE_HTTP_PROTO);
    String *body = String_Make(m, bytes("Wonderful-Things-To-Come!"));
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("POST /update HTTP/1.2\r\nUser-Agent: Firefox\r\nCookie: sid=xyz1234;Expiration=2024-04-04;Secure=true;\r\n")));
    String_AddBytes(req->m, req->in.shelf, bytes("Content-Length: "), strlen("Content-Length: "));
    String_Add(req->m, req->in.shelf, String_FromInt(m, body->length));
    String_AddBytes(req->m, req->in.shelf, bytes("\r\n"), 2);
    String_AddBytes(req->m, req->in.shelf, bytes("\r\n"), 2);
    String_Add(req->m, req->in.shelf, body);

    printf("\n");
    Debug_Print((void *)req->in.shelf, 0, "Req for post request: ", COLOR_DARK, TRUE);
    printf("\n");

    /*
    Roebling_Run(req->in.rbl);
    */

    return r;
}
