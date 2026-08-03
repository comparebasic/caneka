#include <external.h>
#include <caneka.h>

status Req_Prepare(MemCh *m, Req *req, Serve *srv){
    return req->def->prepare(m, req, srv);
}

void Req_Handle(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    Single *sg = NULL;
    Iter *it = &req->chain;
    if(req->type.state & ERROR){
        sg = Iter_GetByIdx(&req->chain, 0);
    }else{
        if((req->chain.type.state & PROCESSING) == 0){
            Iter_Init(&req->chain, Span_Make(m));
            Iter it;
            Iter_Init(&it, req->def->route);
            while((Iter_Prev(&it) & END) == 0){
                Iter_Add(&req->chain, Iter_Get(&it));
            }
            Single *sg = Iter_Get(&it);
        }else if(req->type.state & SUCCESS){
            Abstract *a = Iter_Get(&it);
            /* TODO: remove empties */
            while(a->type.of == TYPE_ITER){
                it = (Iter *)a;
                Abstract *a = Iter_Get(&it);
                if(a->type.of == TYPE_WRAPPED_FUNC){
                    sg = (Single *)a;
                    break;
                }
            }
        }
    }

    Single *sg = Iter_Get(&req->chain);
    ReqFunc func = (ReqFunc)sg->val.ptr;

    req->type.state = req->type.state & (DEBUG|END|ERROR|NOOP|PROCESSING);
    func(m, req, srv);
    if(req->type.state & (SUCCESS|ERROR)){
        Iter_Remove(&it);
        Iter_Prev(&it);
    }

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
