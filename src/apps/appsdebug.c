#include <external.h>
#include <caneka.h>

static void ProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sProtoDef:\x1b[0m", color, msg);
    Debug_Print((void *)proto, proto->reqType, msg, color, extended);
}

static void Req_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Req *req = (Req *) as(a, TYPE_REQ);
    printf("%s\x1b[1;%dmReq<%s ", msg, color, State_ToString(req->type.state));
    Debug_Print((void *)req->proto, 0, "", color, extended);
    if(req->in.rbl != NULL && extended){
        printf(" ");
        Debug_Print((void *)req->in.rbl, 0, "", color, extended);
    }
    printf(">\x1b[0m");
}

static void XmlCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    XmlCtx *ctx = (XmlCtx *)as(a, TYPE_XMLCTX);
    if(extended){
        printf("\x1b[%dm%sXmlCtx<%s nodes=", color, msg, State_ToString(ctx->type.state));
        Debug_Print((void *)ctx->root, 0, "", color, FALSE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sXmlCtx<%s>\x1b[0m", color, msg, State_ToString(ctx->type.state));
    }
}

static void HttpProto_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    HttpProto *proto = (HttpProto *)as(a, TYPE_HTTP_PROTO);
    printf("\x1b[%dmHttpProto< %s", color, proto->path != NULL ? (char *)String_ToEscaped(DebugM, proto->path)->bytes : "");
    Debug_Print(proto->body, 0, "body=", color, TRUE);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void HttpProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sHTTP<>\x1b[0m", color, "");
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_XMLCTX, (void *)XmlCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTODEF, (void *)HttpProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTODEF, (void *)ProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    return r;
}

status AppsDebug_Init(MemCtx *m){
    Lookup *funcs = Lookup_Make(m, _TYPE_APPS_START, populateDebugPrint, NULL);
    return Chain_Extend(m, DebugPrintChain, funcs);
}
