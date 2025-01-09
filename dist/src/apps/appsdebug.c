#include <external.h>
#include <caneka.h>

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

static void XmlCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    XmlCtx *ctx = (XmlCtx *)as(a, TYPE_XMLCTX);
    if(extended){
        printf("\x1b[%dm%sXmlCtx<%s nodes=", color, msg, State_ToChars(ctx->type.state));
        Debug_Print((void *)ctx->root, 0, "", color, FALSE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sXmlCtx<%s>\x1b[0m", color, msg, State_ToChars(ctx->type.state));
    }
}

static void HttpProto_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    HttpProto *proto = (HttpProto *)as(a, TYPE_HTTP_PROTO);
    printf("\x1b[%dmHttpProto<%s path='%s' ", color, HttpProto_MethodToChars(proto->method), proto->path != NULL ? (char *)String_ToEscaped(DebugM, proto->path)->bytes : "");
    Debug_Print(proto->body, 0, "body=", color, TRUE);
    Debug_Print(proto->headers_tbl, 0, " headers=", color, TRUE);
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
    r |= Lookup_Add(m, lk, TYPE_SERVECTX, (void *)Serve_Print);
    return r;
}

char *AppRange_ToChars(word range){
    if(range == _APP_BOUNDRY_START){
        return "_APP_BOUNDRY_START";
    }else if(range == _IO_START){
        return "_IO_START";
    }else if(range == IO_RECV){
        return "IO_RECV";
    }else if(range == IO_RUN){
        return "IO_RUN";
    }else if(range == IO_DONE){
        return "IO_DONE";
    }else if(range == _IO_END){
        return "_IO_END";
    }else if(range == _CASH_START){
        return "_CASH_START";
    }else if(range == CASH_MARK_START){
        return "CASH_MARK_START";
    }else if(range == CASH_BETWEEN){
        return "CASH_BETWEEN";
    }else if(range == CASH_VALUE){
        return "CASH_VALUE";
    }else if(range == CASH_NOOP){
        return "CASH_NOOP";
    }else if(range == _CASH_END){
        return "_CASH_END";
    }else if(range == _RBLSH_START){
        return "_RBLSH_START";
    }else if(range == RBLSH_MARK_START){
        return "RBLSH_MARK_START";
    }else if(range == RBLSH_MARK_END){
        return "RBLSH_MARK_END";
    }else if(range == RBLSH_TERM){
        return "RBLSH_TERM";
    }else if(range == RBLSH_HUP){
        return "RBLSH_HUP";
    }else if(range == RBLSH_WS){
        return "RBLSH_WS";
    }else if(range == RBLSH_NL){
        return "RBLSH_NL";
    }else if(range == RBLSH_ARG){
        return "RBLSH_ARG";
    }else if(range == RBLSH_FLAG){
        return "RBLSH_FLAG";
    }else if(range == RBLSH_WORDFLAG){
        return "RBLSH_WORDFLAG";
    }else if(range == RBLSH_OP){
        return "RBLSH_OP";
    }else if(range == RBLSH_STRING_LIT){
        return "RBLSH_STRING_LIT";
    }else if(range == RBLSH_STRING_OP){
        return "RBLSH_STRING_OP";
    }else if(range == _RBLSH_END){
        return "_RBLSH_END";
    }else if(range == _APPS_BOUNDRIES_START){
        return "_APPS_BOUNDRIES_START";
    }else if(range == _APPS_CASH_START){
        return "_APPS_CASH_START";
    }else if(range == _APPS_RBLSH_START){
        return "_APPS_RBLSH_START";
    }else if(range == _APPS_TYPE_START){
        return "_APPS_TYPE_START";
    }else if(range == _TYPE_APPS_START){
        return "_TYPE_APPS_START";
    }else if(range == TYPE_RBLSH_CTX){
        return "TYPE_RBLSH_CTX";
    }else if(range == TYPE_SERVECTX){
        return "TYPE_SERVECTX";
    }else if(range == TYPE_REQ){
        return "TYPE_REQ";
    }else if(range == TYPE_PROTO){
        return "TYPE_PROTO";
    }else if(range == TYPE_PROTODEF){
        return "TYPE_PROTODEF";
    }else if(range == TYPE_XMLCTX){
        return "TYPE_XMLCTX";
    }else if(range == TYPE_HTTP_PROTO){
        return "TYPE_HTTP_PROTO";
    }else if(range == TYPE_HTTP_PROTODEF){
        return "TYPE_HTTP_PROTODEF";
    }else if(range == _TYPE_HTTP_METHODS_START){
        return "_TYPE_HTTP_METHODS_START";
    }else if(range == TYPE_METHOD_GET){
        return "TYPE_METHOD_GET";
    }else if(range == TYPE_METHOD_POST){
        return "TYPE_METHOD_POST";
    }else if(range == TYPE_METHOD_SET){
        return "TYPE_METHOD_SET";
    }else if(range == TYPE_METHOD_UPDATE ){
        return "TYPE_METHOD_UPDATE ";
    }else if(range == TYPE_METHOD_DELETE){
        return "TYPE_METHOD_DELETE";
    }else if(range == TYPE_IO_PROTO){
        return "TYPE_IO_PROTO";
    }else if(range == TYPE_IO_PROTODEF){
        return "TYPE_IO_PROTODEF";
    }else if(range == _TYPE_APPS_END){
        return "_TYPE_APPS_END";
    }else{
        return "UNKNOWN";
    }
};

status AppsDebug_Init(MemCtx *m){
    Lookup *funcs = Lookup_Make(m, _TYPE_APPS_START, populateDebugPrint, NULL);
    return Chain_Extend(m, DebugPrintChain, funcs);
}
