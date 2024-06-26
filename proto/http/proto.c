#include <external.h>
#include <filestore.h>
#include <proto/http.h>

Vitrual *Http_ProtoMake(MemCtx *m, Serve *sctx){
    Maker rbl_mk = (Maker) Proto_Get(sctx->protoMakers_mk, TYPE_HTTP_PROTO);
    if(rbl_mk == NULL){
        Fatal("proto maker not found");
        return NULL;
    }

    HttpProto *p = (HttpProto *)MemCtx_Alloc(m, sizeof(HttpProto));
    p->type.of = TYPE_HTTP_PROTO;
    p->shelf = String_Init(m, STRING_EXTEND);
    p->rbl = Roebling_Make(m, rbl_mk);

    return p;
}

static status populateMethods(MemCtx *m, RlbLookup *lk){
    status r = READY;
    r |= Span_Set(m, lk->values, TYPE_METHOD_GET-lk->offset, String_Make(m, bytes("GET"));
    r |= Span_Set(m, lk->values, TYPE_METHOD_POST-lk->offset, String_Make(m, bytes("POST"));
    r |= Span_Set(m, lk->values, TYPE_METHOD_SET-lk->offset, String_Make(m, bytes("SET"));
    r |= Span_Set(m, lk->values, TYPE_METHOD_UPDATE-lk->offset, String_Make(m, bytes("UPDATE"));
    return r;
}

static Virtual *HttpReq_Make(Memctx *m, Virtual *_sctx){
    if(_sctx->type.of != TYPE_SERVECTX){
        Fatal("incorrect type found %ld", _sctx->type.of);
        return NULL;
    }
    Serve *sctx = (Serve *)_sctx;

    MemCtx *m = MemCtx_Make();
    HttpProto *proto = HttpProto_Make(m, sctx);;
    Req *req =  Req_Make(m, sctx, (Proto *)proto, -1);
    MemCtx_Bind(m, req);

    return Req_Make(m, );
}

ProtoDef *Http_ProtoDefMake(MemCtx *m, Serve *sctx){
    Lookup *methods = RlbLookup_Make(m, _TYPE_HTTP_START, populateMethods, NULL);
    return ProtoDef_Make(m, NULL, methods, NULL); 
}
