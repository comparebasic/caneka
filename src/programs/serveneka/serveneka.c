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

#ifdef CNKOPT_CRYPTO
    Str *tlsCert = Inst_Att(config, K(m, "tls-cert"));
    Str *tlsKey = Inst_Att(config, K(m, "tls-key"));
    Str *cmdFile = Inst_Att(config, K(m, "cmd-file"));


    if(tlsCert != NULL && tlsKey != NULL){
        Tls_Init(m);

        ent->ctx = TlsCtx_Make(m, IoPath_From(m, tlsCert), IoPath_From(m, tlsKey));
        void *ar[] = {
            ent->ctx, NULL
        };

        args[0] = tlsCert;
        args[1] = tlsKey;
        args[2] = ent->ctx;
        args[3] = NULL;
        Out("^b.Tls cert:@ key:@ -> @^0\n", args);
    }
#endif

    Serve *srv = Serve_MakeTcp(m, def, ent);
    srv->config = config;

    if(cmdFile != NULL && tlsKey != NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        if(File_Open(bf, IoUtil_GetAbsPath(m, cmdFile), O_RDONLY|O_CREAT) & SUCCESS){
            HttpStatic_SetCmdFile(m, srv, file);
        }
    }

    args[0] = config->m;
    args[1] = NULL;
    Out("^c.Config @^0\n", args);

    Serve_ServeTcp(srv);
}
