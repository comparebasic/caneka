#include <external.h>
#include <caneka.h>

Serve *Serve_Make(MemCh *m, IoCtx *ctx){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m);
    return srv;
}
