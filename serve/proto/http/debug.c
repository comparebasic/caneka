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
    HttpProto *proto = (HttpProto *) as(t, TYPE_HTTP_PROTO);
    printf("HttpProto<%s \x1b[1m%s\x1b[0;%dm", Method_ToString(proto->method), proto->path->bytes, color);
    Debug_Print((void *)proto->headers_tbl, 0, " Headers:", color, extended);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void HttpProtoDef_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(((Abstract *)t)->type.state != TYPE_HTTP_PROTODEF){
        Fatal("Incorrect type", TYPE_HTTP_PROTODEF);
        return;
    }
    ProtoDef *def = (ProtoDef *)t;
    printf("\x1b[%dmHttpProtoDef<HTTP", color);
    if(def->methods != NULL){
        printf(", method:%d", def->methods->values->nvalues);
        /*
        Debug_Print((void *)def->methods->values, def->methods->values->type.of, "", color, extended);
        */
    }
    if(def->parsers_pmk != NULL){
        printf(", parsers:%d", def->parsers_pmk->nvalues);
        /*
        Debug_Print((void *)def->methods->values, def->methods->values->type.of, "", color, extended);
        */
    }
    printf(">\x1b[0m");
}

static status populateHttpDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTODEF, (void *)HttpProtoDef_Print);
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
