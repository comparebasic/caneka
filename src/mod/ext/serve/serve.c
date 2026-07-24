#include <external.h>
#include <caneka.h>

void Serve_LogOpen(Serve *srv, Inst *tsk){
    HandlerDef *def = Span_Get(tsk, TASK_DEF);
    def->log.open(srv->m, req, srv);
}

void Serve_LogFinalized(Serve *srv, Inst *tsk){
    HandlerDef *def = Span_Get(tsk, TASK_DEF);
    def->log.final(srv->m, req, srv);
}

Serve *Serve_Make(MemCh *m, Table *routes /* <HostEnt, HandlerDef> */, void *source){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m, (QueueCritFunc)TaskCrit_Func);
    srv->source = source;
    srv->routes = routes;
    srv->log.out = OutStream;

    return srv;
}
