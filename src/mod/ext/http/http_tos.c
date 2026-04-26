#include <external.h>
#include <caneka.h>

static status HttpReq_Print(Buff *bf, void *a, cls type, word flags){
    HttpReq *req = (HttpReq *)a;
    void *args[] = {
        HttpMethodStr(bf->m, req->address.method),
        NULL,
    };
    return Fmt(bf, "HttpReq<$>", args);
}

Str *HttpMethodStr(MemCh *m, cls method){
    if(method == METHOD_HTTP_GET){
        return S(m, "GET");
    }else if(method == METHOD_HTTP_POST){
        return S(m, "POST");
    }else if(method == METHOD_HTTP_DELETE){
        return S(m, "DELETE");
    }else if(method == METHOD_HTTP_PUT){
        return S(m, "PUT");
    }else{
        return S(m,"UNKONWN");
    }
}

status Http_TosInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_HTTP_REQ, (void *)HttpReq_Print);
    return r;
}
