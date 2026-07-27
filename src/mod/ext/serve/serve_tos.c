#include <external.h>
#include <caneka.h>


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
        args[2] = NULL;
    }else if(addr->type.of == TYPE_NET_ADDR6){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Ipv6 not yet supported ", NULL);
        return ERROR;
    }else{
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unknown address type", NULL);
        return ERROR;
    }

    return Fmt(bf, "$=$", args);
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
        NULL,
    };
    Fmt(bf, "HostEnt<@ $ ", ar);

    if(h->addr != NULL && h->addr->type.of == TYPE_WRAPPED_U32){
        Single *sg = (Single *)h->addr;
        Str *s = Ip4_ToStr(m, sg->val.i);
        Buff_AddBytes(bf, s->bytes, s->length);
    }

    Buff_AddBytes(bf, (byte *)">", 1);
    
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
    return r;
}
