#include <external.h>
#include <caneka.h>

void *FileReq_SourceMake(MemCh *m, Serve *srv, HandlerDef *def){

    /*
    HttpReq *req = rm->owner = (HttpReq *)srv->def->mk(rm, srv);
    req->key = S(m, "cmd");
    Time_Now(&req->metrics.start);

    req->crit = ReqCrit_Make(m);
    req->crit->pfd.fd = file->fd;
    req->idx = Queue_Add(srv->q, req, req->crit);
    HttpReq_ExpectRecv(req);
    */
    return NULL;
}
