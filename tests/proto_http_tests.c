#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status ProtoHttp_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    HttpProto_Init(m);
    Serve *sctx = Serve_Make(m, HttpProtoDef_Make(m));
    Req *req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    HttpProto *proto = (HttpProto *) as(req->proto, TYPE_HTTP_PROTO);
    Debug_Print((void *)req, 0, "Req in prto test: ", COLOR_CYAN, TRUE);
    printf("\n");
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("GET /page1.html HTTP/1.1\r\nUser-Agent: Firefox\r\n\r\n")));
    Roebling_Run(req->in.rbl);

    String *method_s = Lookup_Get(sctx->def->methods, proto->method);
    r |= Test(proto->method == TYPE_METHOD_GET, "Expect method to be set to get found %d(%s)", proto->method, method_s->bytes);
    r |= Test(String_Equals(proto->path, String_Make(m, bytes("/page1.html"))), "Expect string path to equal have '%s'", proto->path->bytes);

    return r;
}
