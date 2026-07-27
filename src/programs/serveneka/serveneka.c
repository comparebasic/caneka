#include <external.h>
#include <caneka.h>

void Serveneka_Serve(MemCh *m, Node *config){
    void *args[5];

    args[0] = config;
    args[1] = NULL;
    Out("^p.Config: $^0\n", args);

    /*
    HttpStatic_Init(m);
    HandlerDef *def = HttpStatic_DefMake(m);

    Table *routes = Table_Make(m);
    HandlerDef *def = HttpStatic_DefMake(m);
    Table_Set(routes, ent, def);
    TcpSource *source = TcpSource_Make(m);

    Serve *srv = Serve_Make(m, Table_Ordered(m, routes), source);
    srv->config = config;

    if(cmdFile != NULL && tlsKey != NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        if(File_Open(bf, IoUtil_GetAbsPath(m, cmdFile), O_RDONLY|O_CREAT) & SUCCESS){
            HttpStatic_SetCmdFile(m, srv, file);
        }
    }

    Serve_ServeTcp(srv);
    */
}
