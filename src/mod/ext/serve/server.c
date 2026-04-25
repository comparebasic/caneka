#include <external.h>
#include <caneka.h>

Server *Server_Make(MemCh *m, IoCtx *ctx){
    Server *srv = MemCh_AllocOf(m, sizeof(Server), TYPE_SERVER);
    srv->type.of = TYPE_SERVER;
    srv->m = m;
    srv->q = Queue_Make(m);
    return srv;
}
