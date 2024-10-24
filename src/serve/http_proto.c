#include <external.h>
#include <caneka.h>

static char *toLog(Req *req){
    HttpProto *proto = (HttpProto *)req->proto;
    String *s = String_Init(req->m, STRING_EXTEND);
    String_AddBytes(req->m, s, bytes("HttpProto<"), strlen("HttpProto<"));
    char *state = State_ToString(req->type.state);
    String_AddBytes(req->m, s, bytes(state), strlen(state));
    String_AddBytes(req->m, s, bytes(" "), 1);
    char *method = HttpProto_MethodToChars(proto->method);
    String_AddBytes(req->m, s, bytes(method), strlen(method));
    String_AddBytes(req->m, s, bytes(" "), 1);
    String_Add(req->m, s, proto->path);
    String_AddBytes(req->m, s, bytes(">"), 1);

    return (char *)s->bytes;
}

static status reqHttpParser_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    HttpProto *proto = (HttpProto *)source;
    if(captureKey == HTTP_METHOD){
        proto->method = Lookup_GetKey(proto->methods, matchIdx);
    }else if(captureKey == HTTP_PATH){
        proto->path = s;
    }else if(captureKey == HTTP_HEADER){
        Table_SetKey(proto->headers_tbl, (Abstract *)s);
    }else if(captureKey == HTTP_HEADER_VALUE){
        Table_SetValue(proto->headers_tbl, (Abstract *)s);
    }

    if(DEBUG_HTTP){
        Debug_Print(proto, 0, "HttpCaptured: ", DEBUG_HTTP, TRUE);
        printf("\n");
    }

    status r = SUCCESS;
    return r;
}

static char *codeChars(int code){
    if(200){
        return "200 OK";
    }else if(404){
        return "404 NOT FOUND";
    }else if(403){
        return "404 FORBIDDEN";
    }else if(302){
        return "302 REDIRECT";
    }else if(301){
        return "301 REDIRECT";
    }else{
        return "500 ERROR";
    }
}

status Req_SetResponse(Req *req, int code, Span *headers, String *body){
   String *resp = String_Init(req->m, STRING_EXTEND); 
   String_AddBytes(req->m, resp, bytes("HTTP/1.1 "), strlen("HTTP/1.1 "));
   char *code_cstr = codeChars(code);
   String_AddBytes(req->m, resp, bytes(code_cstr), strlen(code_cstr));
   String_AddBytes(req->m, resp, bytes("\r\n"), 2);
   String_AddBytes(req->m, resp, bytes("Content-Length: "), strlen("Content-Length: "));
   String_Add(req->m, resp, String_FromInt(req->m, String_Length(body)));
   String_AddBytes(req->m, resp, bytes("\r\n\r\n"), 4);
   String_Add(req->m, resp, body);

   req->out.response = resp;
   return SUCCESS;
}

Proto *HttpProto_Make(MemCtx *m, ProtoDef *def){
    HttpProto *p = (HttpProto *)MemCtx_Alloc(m, sizeof(HttpProto));
    p->type.of = TYPE_HTTP_PROTO;
    p->toLog = toLog;
    p->headers_tbl = Span_Make(m, TYPE_TABLE);
    p->methods = def->methods;

    return (Proto *)p;
}

Req *HttpReq_Make(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    HttpProto *proto = (HttpProto *)HttpProto_Make(m, (ProtoDef *)sctx->def);
    Req *req =  Req_Make(m, sctx, (Proto *)proto);
    req->in.rbl = HttpParser_Make(m, req->in.shelf, (Abstract *)proto);
    req->in.rbl->capture = reqHttpParser_Capture; 
    MemCtx_Bind(m, req);

    return req;
}

ProtoDef *HttpProtoDef_Make(MemCtx *m){
    Lookup *lk = Lookup_Make(m, HTTP_METHOD_GET, NULL, NULL); 
    Lookup_Add(m, lk, HTTP_METHOD_GET, 
         (void *)String_Make(m, bytes("GET")));
    Lookup_Add(m, lk, HTTP_METHOD_POST, 
         (void *)String_Make(m, bytes("POST")));
    Lookup_Add(m, lk, HTTP_METHOD_UPDATE, 
         (void *)String_Make(m, bytes("UPDATE")));
    Lookup_Add(m, lk, HTTP_METHOD_SET, 
         (void *)String_Make(m, bytes("SET")));
    Lookup_Add(m, lk, HTTP_METHOD_DELETE, 
         (void *)String_Make(m, bytes("DELETE")));

    return ProtoDef_Make(m, TYPE_HTTP_PROTODEF,
        (Maker)HttpReq_Make,
        (Maker)HttpProto_Make,
        lk
    ); 
}
