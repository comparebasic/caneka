#include <external.h>
#include <caneka.h>

void Req_Handle(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;
    if(req->type.state & ERROR){
        Single *sg = Iter_GetByIdx(&req->chain, req->chain.p->max_idx);
    }else{
        if((req->chain.type.state & PROCESSING) == 0){
            Iter_Init(&req->chain, req->def->route);
            Iter_Next(&req->chain);
        }else if(req->type.state & SUCCESS){
            Iter_Next(&req->chain);
        }
    }

    req->type.state = req->type.state & (DEBUG|END|ERROR|NOOP|PROCESSING);
    do {
        Single *sg = Iter_Get(&req->chain);
        ReqFunc func = (ReqFunc)sg->val.ptr;
        func(m, req, srv);
    }while(req->type.state & MORE);

    ReturnVoid(m);
}

void Req_ExpectRecv(Req *req){
    req->crit->pfd.events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
}

void Req_ExpectInternal(Req *req){
    req->crit->pfd.events = POLLNVAL|POLLHUP|POLLERR;
}

void Req_ExpectSend(Req *req){
    req->crit->pfd.events = POLLOUT|POLLNVAL|POLLHUP|POLLERR;
}

Req *Req_Make(MemCh *m, HandlerDef *def, NetAddr *addr, i32 fd, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_REQ);
    req->m = m;
    req->type.of = TYPE_REQ;
    req->def = def;
    req->addr = addr;
    req->crit = ReqCrit_Make(m);
    req->crit->pfd.fd = fd;
    req->source = source;
    void *ar[] = {req->source, NULL};
    Out("^p.HttpReq @^0\n", ar);

    def->setup(m, req);

    return req;
}
