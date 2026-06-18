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

static status IoCtx_Print(Buff *bf, void *a, cls type, word flags){
    IoCtx *ctx = (IoCtx*)a;
    void *args[] = {
        NULL,
    };
    return Fmt(bf, "IoCtx<>", args);
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
        S(m, h->ent->h_name),
        NULL,
    };
    Fmt(bf, "HostEnt<@ $ ", ar);

    Iter it;
    Iter_Init(&it, h->addrs);
    while((Iter_Next(&it) & END) == 0){
        Single *sg = Iter_Get(&it);
        Str *s = Ip4_ToStr(m, sg->val.i);
        if(it.idx > 0){
            Buff_AddBytes(bf, (byte *)", ", 2);
        }
        Buff_AddBytes(bf, s->bytes, s->length);
    }

    Buff_AddBytes(bf, (byte *)">", 1);
    
    return ZERO;
}

status Serve_TosInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_IO_CTX, (void *)IoCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_SERVE, (void *)Serve_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    r |= Lookup_Add(m, lk, TYPE_HOST_ENT, (void *)HostEnt_Print);
    return r;
}
