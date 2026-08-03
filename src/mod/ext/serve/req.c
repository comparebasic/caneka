#include <external.h>
#include <caneka.h>

void Req_Handle(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
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

    req->type.state = req->type.state & (DEBUG|END|ERROR|NOOP|PROCESSING);
    func(m, req, srv);
    if(req->type.state & (SUCCESS|ERROR)){
        Iter_Remove(&it);
        Iter_Prev(&it);
    }

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
