#include <external.h>
#include <caneka.h>

static char *toLog(Req *req){
    return "HttpProto<>";
}

Proto *HttpProto_Make(MemCtx *m){
    HttpProto *p = (HttpProto *)MemCtx_Alloc(m, sizeof(HttpProto));
    p->type.of = TYPE_HTTP_PROTO;
    p->toLog = toLog;
    p->headers_tbl = Span_Make(m, TYPE_TABLE);

    return (Proto *)p;
}

Req *HttpReq_Make(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    HttpProto *proto = (HttpProto *)HttpProto_Make(m);
    Req *req =  Req_Make(m, sctx, (Proto *)proto, -1);
    req->in.rbl = HttpParser_Make(m, req->in.shelf, (Abstract *)proto);
    MemCtx_Bind(m, req);

    return req;
}

ProtoDef *HttpProtoDef_Make(MemCtx *m, Lookup *handlers){
    return ProtoDef_Make(m, TYPE_HTTP_PROTODEF,
        (Maker)HttpReq_Make,
        (Maker)HttpProto_Make,
        handlers
    ); 
}
