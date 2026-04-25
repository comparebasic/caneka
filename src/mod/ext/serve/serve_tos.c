#include <external.h>
#include <caneka.h>

static StrVec *getPollFlagVec(MemCh *m, struct pollfd *pfd){
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

static status Server_Print(Buff *bf, void *a, cls type, word flags){
    Server *ctx = (Server*)a;
    void *args[] = {
        NULL,
    };
    return Fmt(bf, "Server<>", args);
}

static status Req_Print(Buff *bf, void *a, cls type, word flags){
    Server *ctx = (Server*)a;
    void *args[] = {
        NULL,
    };
    return Fmt(bf, "Server<>", args);
}

status Serve_TosInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_IO_CTX, (void *)IoCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_SERVER, (void *)Server_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    return r;
}
