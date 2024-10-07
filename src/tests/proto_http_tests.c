#include <external.h>
#include <caneka.h>

static Abstract *hdrCookieProcess(MemCtx *source, Abstract *a){
    Req *req = (Req *) as(source, TYPE_REQ);
    MemCtx *m = MemCtx_FromHandle((MemHandle *)req);
    String *s = (String *)asIfc(a, TYPE_STRING);
    HttpProto *proto = (HttpProto*)req->proto;
    proto->sid = s;
    return (Abstract *)s;
}

status ProtoHttp_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    HttpProto_Init(m);

    Req *req = NULL;
    HttpProto *proto = NULL;

    ProtoDef *def = HttpProtoDef_Make(m);
    Table_Set(def->hdrHandlers_tbl_mk, (Abstract *)String_Make(m, bytes("Content-Length")),
        (Abstract *)Maker_Wrapped(m, Hdr_IntMk)); 
    Table_Set(def->hdrHandlers_tbl_mk, (Abstract *)String_Make(m, bytes("Cookie")),
        (Abstract *)Maker_Wrapped(m, hdrCookieProcess)); 

    Serve *sctx = Serve_Make(m, def);

    req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    proto = (HttpProto *) as(req->proto, TYPE_HTTP_PROTO);
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("GET /page1.html HTTP/1.2\r\nUser-Agent: Firefox\r\nCookie: sid=xyz1234;Expiration=2024-04-04;Secure=true;\r\nAccept: text/html,application/json\r\n\r\n")));

    Roebling_RunCycle(req->in.rbl);

    r |= Test(req->in.rbl->type.state == SUCCESS, "Expect roebling to be SUCCESS have %s", State_ToString(req->in.rbl->type.state));
    String *method_s = Lookup_Get(sctx->def->methods, proto->method);
    r |= Test(proto->method == TYPE_METHOD_GET, "Expect method to be set to get found %d(%s)", proto->method, method_s->bytes);
    r |= Test(String_Equals(proto->path, String_Make(m, bytes("/page1.html"))), "Expect string path to equal have '%s'", proto->path->bytes);
    r |= Test(String_Equals(proto->sid, String_Make(m, bytes("sid=xyz1234;Expiration=2024-04-04;Secure=true;"))), "Expect string cookie to equal have '%s'", proto->sid->bytes);

    req = (Req *)sctx->def->req_mk(sctx->m, (Abstract *)sctx);
    proto = (HttpProto *) as(req->proto, TYPE_HTTP_PROTO);
    String *body = String_Make(m, bytes("Wonderful-Things-To-Come!"));
    String_Add(req->m, req->in.shelf, String_Make(m, bytes("POST /update HTTP/1.2\r\nUser-Agent: Firefox\r\nCookie: sid=xyz1234;Expiration=2024-04-04;Secure=true;\r\n")));
    String_AddBytes(req->m, req->in.shelf, bytes("Content-Length: "), strlen("Content-Length: "));
    String_Add(req->m, req->in.shelf, String_FromInt(m, body->length));
    String_AddBytes(req->m, req->in.shelf, bytes("\r\n"), 2);
    String_AddBytes(req->m, req->in.shelf, bytes("\r\n"), 2);
    String_Add(req->m, req->in.shelf, body);

    Roebling_RunCycle(req->in.rbl);
    r |= Test(req->in.rbl->type.state == SUCCESS, "Expect roebling to be SUCCESS have %s", State_ToString(req->in.rbl->type.state));

    method_s = Lookup_Get(sctx->def->methods, proto->method);
    r |= Test(proto->method == TYPE_METHOD_POST, "Expect method to be set to get found %d(%s)", proto->method, method_s->bytes);
    r |= Test(String_Equals(proto->path, String_Make(m, bytes("/update"))), "Expect string path to equal have '%s'", proto->path->bytes);
    Abstract *contentLengthHdr = Table_Get(req->proto->headers_tbl, (Abstract *)String_Make(m, bytes("Content-Length")));
    r |= Test(contentLengthHdr->type.of == TYPE_WRAPPED_UTIL, "Expect Content-Length to be a wrapped util");
    Single *sgl = (Single *)contentLengthHdr;
    r |= Test(String_Equals(proto->sid, String_Make(m, bytes("sid=xyz1234;Expiration=2024-04-04;Secure=true;"))), "Expect string cookie to equal have '%s'", proto->sid->bytes);
    r |= Test(sgl->val.value == body->length,  "Expect content length to have value of body length %u found %u", body->length, sgl->val.value);
    r |= Test(String_Equals(proto->body, body), "Body found expected '%s' have '%s'", body->bytes, proto->body->bytes);

    return r;
}
