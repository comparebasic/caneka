#include <external.h>
#include <caneka.h>
#include <proto/http.h>

status HttpProto_Init(MemCtx *m){
    return HttpProtoDebug_Init(m);
}

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
    r |= Lookup_Add(m, lk, TYPE_METHOD_GET, (Abstract *)String_Make(m, bytes("GET")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_POST, (Abstract *)String_Make(m, bytes("POST")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_SET, (Abstract *)String_Make(m, bytes("SET")));
    r |= Lookup_Add(m, lk, TYPE_METHOD_UPDATE, (Abstract *)String_Make(m, bytes("UPDATE")));
    return r;
}

Req *HttpReq_Make(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    HttpProto *proto = (HttpProto *)HttpProto_Make(m, sctx);;
    Req *req =  Req_Make(m, sctx, (Proto *)proto, -1);
    MemCtx_Bind(m, req);

    return req;
}

ProtoDef *HttpProtoDef_Make(MemCtx *m){
    Lookup *methods = Lookup_Make(m, _TYPE_HTTP_START, populateMethods, NULL);
    return ProtoDef_Make(m, NULL, methods, NULL); 
}
