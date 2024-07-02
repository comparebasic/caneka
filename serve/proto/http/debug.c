#include <external.h>
#include <caneka.h>
#include <proto/http.h>

static Chain *debugChain = NULL;

static char *Method_ToString(int method){
    if(method == TYPE_METHOD_GET){
        return "GET";
    }else if(method == TYPE_METHOD_SET){
        return "SET";
    }else if(method == TYPE_METHOD_UPDATE){
        return "UPDATE";
    }else{
        return "UNKONWN_method";
    }
}

static void HttpProto_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(((Abstract *)t)->type.of != TYPE_HTTP_PROTO){
        Fatal("Incorrect type", TYPE_HTTP_PROTO);
        return;
    }
    HttpProto *proto = (HttpProto *)t;
    printf("Http<%s %s>", Method_ToString(proto->method), proto->path->bytes);
}

static status populateHttpDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    return r;
}

status HttpProtoDebug_Init(MemCtx *m){
    if(debugChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_PROTO_START, populateHttpDebugPrint, NULL);
        Chain_Extend(m, DebugPrintChain, funcs); 
        return SUCCESS;
    }
    return NOOP;
}
