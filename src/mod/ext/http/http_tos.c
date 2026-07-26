#include <external.h>
#include <caneka.h>

static status HttpReq_Print(Buff *bf, void *a, cls type, word flags){
    HttpReq *req = (HttpReq *)a;
    MemCh *m = bf->m;
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(m, req->type.of, req->type.state),
            HttpMethodStr(m, req->address.method),
            req->path,
            req->headersIt.p,
            req->queryIt.p,
            req->body,
            req->meta,
            req->sections,
            req->in,
            req->out,
            NULL
        };
        return Fmt(bf, "HttpReq<@ method:$ path:@"
            " headers:@ query:@ body:@ meta:@"
            " sections:@ in:& out:&>", args);
    }else if(flags & MORE){
        void *args[] = {
            Type_StateVec(m, req->type.of, req->type.state),
            HttpMethodStr(m, req->address.method),
            req->path,
            req->headersIt.p,
            req->queryIt.p,
            req->body,
            NULL
        };
        return Fmt(bf, "HttpReq<@ $ @ headers:@ query:@ body:@>", args);
    }else{
        void *args[] = {
            Type_StateVec(m, req->type.of, req->type.state),
            HttpMethodStr(m, req->address.method),
            req->path,
            NULL
        };
        return Fmt(bf, "HttpReq<@ $ @>", args);
    }
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
