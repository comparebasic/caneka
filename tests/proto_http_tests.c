#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status ProtoHttp_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    HttpProto_Init(m);
    Serve *sctx = Serve_Make(m, HttpProtoDef_Make(m));
    Req *req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    Debug_Print((void *)req, 0, "Req in prto test: ", COLOR_CYAN, TRUE);
    printf("\n");
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("GET /page1.html HTTP/1.1\r\n")));
    Roebling_Run(req->in.rbl);

    r |= Test(((HttpProto *)req->proto)->method == TYPE_METHOD_GET, "Expect method to be set to get found %d", ((HttpProto *)req->proto)->method);

    printf("HTTP PROTO\n");

    return r;
}
