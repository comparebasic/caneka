#include <external.h>
#include <caneka.h>

status Req_Prepare(MemCh *m, Req *req, Serve *srv){
    return req->def->prepare(m, req, srv);
}

void Req_Handle(MemCh *m, Req *req, Serve *srv){
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

    Single *sg = Iter_Get(&req->chain);
    ReqFunc func = (ReqFunc)sg->val.ptr;

    req->type.state = req->type.state & (DEBUG|END|ERROR|NOOP|PROCESSING);
    func(m, req, srv);

    return;
}


void Req_SetFd(Req *req, i32 fd){
    req->crit->pfd.fd = fd;
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

Req *Req_Make(MemCh *m, HandlerDef *def, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_REQ);
    req->type.of = TYPE_REQ;
    req->def = def;
    req->source = source;

    return req;
}
