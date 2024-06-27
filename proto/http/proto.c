#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status HttpProto_Init(MemCtx *m){
    HttpProtoDebug_Init(m);
    return SUCCESS;

static char *toLog(Req *req){
    return "Request";
}

Proto *HttpProto_Make(MemCtx *m, Serve *sctx){
    HttpProto *p = (HttpProto *)MemCtx_Alloc(m, sizeof(HttpProto));
    p->type.of = TYPE_HTTP_PROTO;
    p->toLog = toLog;

    return (Proto *)p;
}

static status populateMethods(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Span_Set(m,
        lk->values, TYPE_METHOD_GET-lk->offset, (Abstract *)String_Make(m, bytes("GET")));
    r |= Span_Set(m,
        lk->values, TYPE_METHOD_POST-lk->offset,  (Abstract *)String_Make(m, bytes("POST")));
    r |= Span_Set(m,
        lk->values, TYPE_METHOD_SET-lk->offset,  (Abstract *)String_Make(m, bytes("SET")));
    r |= Span_Set(m,
        lk->values, TYPE_METHOD_UPDATE-lk->offset,  (Abstract *)String_Make(m, bytes("UPDATE")));
    return r;
}

Req *HttpReq_Make(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    HttpProto *proto = (HttpProto *)HttpProto_Make(m, sctx);;
    Req *req =  Req_Make(m, sctx, (Proto *)proto, -1);
    MemCtx_Bind(m, req);

    return req;
}

ProtoDef *HttpProtoDef_Make(MemCtx *m, Serve *sctx){
    Lookup *methods = Lookup_Make(m, _TYPE_HTTP_START, populateMethods, NULL);
    return ProtoDef_Make(m, NULL, methods, NULL); 
}
