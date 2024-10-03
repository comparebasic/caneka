#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status HttpProto_Init(MemCtx *m){
    return HttpProtoDebug_Init(m);
}

static char *toLog(Req *req){
    return "HttpProto<>";
}

Proto *HttpProto_Make(MemCtx *m){
    HttpProto *p = (HttpProto *)MemCtx_Alloc(m, sizeof(HttpProto));
    p->type.of = TYPE_HTTP_PROTO;
    p->toLog = toLog;
    p->headers_tbl = Table_Make(m);

    return (Proto *)p;
}

Req *HttpReq_Make(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    HttpProto *proto = (HttpProto *)HttpProto_Make(m, sctx);
    Req *req =  Req_Make(m, sctx, (Proto *)proto, -1);
    req->in.rbl = HttpParser_Make(m, req->in.shelf, (Abstract *)proto);
    MemCtx_Bind(m, req);

    return req;
}
