#include <external.h>
#include <caneka.h>

void Serve_LogOpen(Serve *srv, Req *req){
    srv->def->log.open(srv->m, req, srv);
}

void Serve_LogFinalized(Serve *srv, Req *req){
    srv->def->log.final(srv->m, req, srv);
}

Serve *Serve_Make(MemCh *m){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m, ReqCrit_Func);
    return srv;
}
