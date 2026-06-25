#include <external.h>
#include <caneka.h>


i32 main(int argc, char **argv){

    status r = READY;
    MemBook *cp = MemBook_Make(NULL);
    void *args[5];
    if(cp == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Core_Direct(m, 1, 2);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *portKey = K(m, "port");
    Str *dirKey = K(m, "dir");
    Str *etagKey = K(m, "etag");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, portKey, S(m, "8000"), ARG_DEFAULT,
        Sv(m, "Port to use."));
    Args_Add(cli, dirKey, NULL, ZERO,
        Sv(m, "Directory to serve files from."));
    Args_Add(cli, etagKey, NULL, ZERO,
        Sv(m, "Directory to store etags."));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, noColorKey)){
        Ansi_SetColor(FALSE);
    }

    Str *portStr = CliArgs_Get(cli, portKey);
    i32 port = Int_FromStr(portStr); 

    HandlerDef *def = HttpStatic_DefMake(m);

    StrVec *dir = IoUtil_GetAbsVec(m, CliArgs_Get(cli, dirKey));
    IoUtil_TrimDir(m, dir);
    Node *config = Inst_Make(m, TYPE_HTTP_CONFIG);

    HostEnt *ent = HostEnt_Make(m);
    ent->port = port;
    Seel_AddTo(config, K(m, "addrs"), NULL, ent);
    Seel_Set(config, K(m, "dir"), dir);
    Seel_Set(config, K(m, "etag"), CliArgs_Get(cli, etagKey));

    Serve *srv = Serve_MakeTcp(m, def, ent);
    srv->config = config;

    args[0] = config->m;
    args[1] = NULL;
    Out("^c.Config @^0\n", args);

    Serve_ServeTcp(srv);

    return (r & ERROR) == 0 ? 0 : 1;
}
