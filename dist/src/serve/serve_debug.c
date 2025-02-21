#include <external.h>
#include <caneka.h>

static void HttpProto_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    HttpProto *proto = (HttpProto *)as(a, TYPE_HTTP_PROTO);
    printf("\x1b[%dmHttpProto<%s path='%s' ", color, HttpProto_MethodToChars(proto->method), proto->path != NULL ? (char *)String_ToEscaped(DebugM, proto->path)->bytes : "");
    Debug_Print(proto->body, 0, "body=", color, FALSE);
    Debug_Print(proto->headers_tbl, 0, " headers=", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void HttpProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sHTTP<>\x1b[0m", color, "");
}


static void ProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sProtoDef:\x1b[0m", color, msg);
    Debug_Print((void *)proto, proto->reqType, msg, color, extended);
}

static void Req_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Req *req = (Req *) as(a, TYPE_REQ);
    printf("%s\x1b[1;%dmReq(%d/%p)<%s ", msg, color, req->fd, req, State_ToChars(req->type.state));
    Debug_Print((void *)req->sctx->def->source, 0, "", color, extended);
    if(req->in.rbl != NULL && extended){
        printf(" ");
        Debug_Print((void *)req->in.rbl, 0, "", color, extended);
    }
    printf(">\x1b[0m");
}

static void Serve_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Serve *sctx = (Serve *) as(a, TYPE_SERVECTX);
    printf("\x1b[%dm%sServe<%s", color, msg, State_ToChars(sctx->type.state));
    if(extended){
        Debug_Print((void *)&sctx->queue, 0, " next=", color, extended);
    }else{
        printf(" count:%d", sctx->queue.count);
    }
    printf(">\x1b[0m");
}


void Handler_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Handler *h = (Handler *)as(a, TYPE_HANDLER);
    if(extended){
        printf("\x1b[%dmH<%d %s", color, h->id, State_ToChars(h->type.state));
    }else{
        printf("\x1b[%dmH<%d %s", color, h->id, State_ToChars(h->type.state));
    }
    if(extended){
        Debug_Print((void *)h->prior, 0, "prior:", color, TRUE);
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

status ServeDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_XMLCTX, (void *)XmlCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTODEF, (void *)HttpProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTODEF, (void *)ProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    r |= Lookup_Add(m, lk, TYPE_SERVECTX, (void *)Serve_Print);
    r |= Lookup_Add(m, lk, TYPE_HANDLER, (void *)Handler_Print);
    return r;
}
