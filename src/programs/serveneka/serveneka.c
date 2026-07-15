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

    Str *tlsCert = Inst_Att(config, K(m, "tls-cert"));
    Str *tlsKey = Inst_Att(config, K(m, "tls-key"));

    if(tlsCert != NULL && tlsKey != NULL){
        Tls_Init(m);

        TlsCtx *ctx = TlsCtx_Make(m, IoPath_From(m, tlsCert), IoPath_From(m, tlsKey));
        void *ar[] = {
            ctx, NULL
        };

        args[0] = tlsCert;
        args[1] = tlsKey;
        args[2] = ctx;
        args[3] = NULL;
        Out("^b.Tls cert:@ key:@ -> @^0\n", args);
    }

    Serve *srv = Serve_MakeTcp(m, def, ent);
    srv->config = config;

    args[0] = config->m;
    args[1] = NULL;
    Out("^c.Config @^0\n", args);

    Serve_ServeTcp(srv);
}
