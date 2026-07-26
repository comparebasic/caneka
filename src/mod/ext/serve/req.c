#include <external.h>
#include <caneka.h>

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

Req *Req_Make(MemCh *m, void *source){
    Req *req = MemCh_AllocOf(m, sizeof(Req), TYPE_TASK_CORE);
    req->type.of = TYPE_REQ;
    req->source = source;

    return req;
}
