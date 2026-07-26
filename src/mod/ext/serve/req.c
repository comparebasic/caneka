#include <external.h>
#include <caneka.h>

void Serve_Handle(MemCh *m, Req *req, Serve *srv){
    if(req->type.state & ERROR){
        Single *sg = Iter_GetByIdx(&req->chain, hreq->chain.p->max_idx);
    }else{
        if((req->routeIt.type.state & PROCESSING) == 0){
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

void Req_Setup(MemCh *m, Serve *srv){
    ;
}

void Req_SetFd(Req *req, i32 fd){
    req->crit->pfd.fd = fd;
}

void Req_ExpectRecv(HttpReq *req){
    req->crit->pfd.events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
}

void Req_ExpectInternal(HttpReq *req){
    req->crit->pfd.events = POLLNVAL|POLLHUP|POLLERR;
}

void Req_ExpectSend(HttpReq *req){
    req->crit->pfd.events = POLLOUT|POLLNVAL|POLLHUP|POLLERR;
}

Req *Req_Make(MemCh *m, HandlerDef *def, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_TASK_CORE);
    req->type.of = TYPE_REQ;
    req->def = def;
    req->source = source;

    return req;
}
