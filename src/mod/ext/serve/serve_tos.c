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

StrVec *Serve_PollFlagVec(MemCh *m, struct pollfd *pfd){
    StrVec *v = StrVec_Make(m);
    if(pfd->events & POLLIN){
        StrVec_Add(v, S(m, "IN"));
    }
    if(pfd->events & POLLOUT){
        StrVec_Add(v, S(m, "OUT"));
    }
    if(poll(pfd, 1, 0)){
        StrVec_Add(v, S(m, "ACTIVE"));
    }
    return v;
}

static status NetAddr_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = bf->m;
    NetAddr *addr = (NetAddr*)a;
    void *args[4];
    if(addr->type.of == TYPE_NET_ADDR4){
        args[0] = S(m, "ip4");
        args[1] = Ip4_ToStr(m, (quad )addr->net.ip4addr.sin_addr.s_addr);
        args[2] = I32_Wrapped(m, addr->port);
        args[3] = NULL;
    }else if(addr->type.of == TYPE_NET_ADDR6){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Ipv6 not yet supported ", NULL);
        return ERROR;
    }else{
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unknown address type", NULL);
        return ERROR;
    }

    return Fmt(bf, "$=$:$", args);
}

static status Serve_Print(Buff *bf, void *a, cls type, word flags){
    Serve *ctx = (Serve*)a;
    void *args[] = {
        NULL,
    };
    return Fmt(bf, "Serve<>", args);
}

static status Req_Print(Buff *bf, void *a, cls type, word flags){
    Serve *ctx = (Serve*)a;
    void *args[] = {
        NULL,
    };
    return Fmt(bf, "Serve<>", args);
}

static status HostEnt_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = bf->m;
    HostEnt *h = (HostEnt *)a;
    void *ar[] = {
        Type_StateVec(m, h->type.of, h->type.state),
        h->name,
        h->addr,
        NULL,
    };
    Fmt(bf, "HostEnt<@ $ $>", ar);
    
    return ZERO;
}

status Serve_TosInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_SERVE, (void *)Serve_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    r |= Lookup_Add(m, lk, TYPE_HOST_ENT, (void *)HostEnt_Print);
    r |= Lookup_Add(m, lk, TYPE_NET_ADDR4, (void *)NetAddr_Print);
    r |= Lookup_Add(m, lk, TYPE_NET_ADDR6, (void *)NetAddr_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_REQ, (void *)HttpReq_Print);
    return r;
}
