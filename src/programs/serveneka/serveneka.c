#include <external.h>
#include <caneka.h>

void Serveneka_Serve(MemCh *m, i32 port, StrVec *dir, Node *config){
    void *args[5];
    HttpStatic_Init(m);
    HandlerDef *def = HttpStatic_DefMake(m);

    HostEnt *ent = HostEnt_Make(m);
    ent->port = port;
    Seel_AddTo(config, K(m, "addrs"), NULL, ent);
    Seel_Set(config, K(m, "dir"), dir);

    Serve *srv = Serve_MakeTcp(m, def, ent);
    srv->config = config;

    args[0] = config->m;
    args[1] = NULL;
    Out("^c.Config @^0\n", args);

    Serve_ServeTcp(srv);
}
