#include <external.h>
#include <caneka.h>

void Req_SetRoute(MemCh *m, Req *req, Serve *srv){
    Abstract *a = NULL; 
    if(req->keys == NULL || req->keys->nvalues == 0 ||
            (a = Inst_GetByPath(req->def->ext, req->keys)) == NULL){
        req->type.state |= ERROR;
        void *ar[] = {req, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error no route keys defined for req @^0\n", ar);
        return;
    };

    if(a->type.of == TYPE_WRAPPED_FUNC){
        Iter_Add(&req->route, a);
        req->type.state |= MORE;
    }else if(a->type.of == TYPE_SPAN){
        Iter_AddSpanRev(&req->route, (Span *)a);  
        req->type.state |= MORE;
    }
}

void Req_Handle(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    if(req->type.state & ERROR){
        return;
    }

    do {
        req->type.state &= ~(MORE|SUCCESS);
        Single *sg = Iter_Get(&req->route);
        ReqFunc func = (ReqFunc)sg->val.ptr;
        func(m, req, srv);
        if(req->type.state & SUCCESS){
            Iter_Remove(&req->route);
            if(Iter_Prev(&req->route) & END){
                break;
            }
        }
    } while(req->type.state & MORE);

    ReturnVoid(m);
}

Req *Req_Make(MemCh *m, HandlerDef *def, NetAddr *addr, i32 fd, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_REQ);
    req->type.of = TYPE_REQ;

    req->m = m;
    req->def = def;
    req->addr = addr;
    req->crit = ReqCrit_Make(m);
    req->crit->pfd.fd = fd;
    req->source = source;

    Iter_Init(&req->route, Span_Make(m));
    Iter_AddSpan(&req->route, def->route);
    def->setup(m, req);

    void *ar[] = {req->source, NULL};
    Out("^p.HttpReq @^0\n", ar);

    return req;
}
