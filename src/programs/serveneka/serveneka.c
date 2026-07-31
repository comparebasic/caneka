#include <external.h>
#include <caneka.h>

void Serveneka_Serve(MemCh *m, Node *config){
    Debug_Push(m, config);
    void *args[5];

    args[0] = config;
    args[1] = NULL;
    Out("^p.Config: $^0\n", args);

    Span *path = Span_Make(m);
    Span_Add(path, K(m, "endpoints"));

    Iter it;
    Inst_IterInitChild(&it, config, path);
    while((Iter_Next(&it) & END) == 0){
        Node *nd = Iter_Get(&it);
        Table *props = Span_Get(nd, INST_PROPIDX_CHILDREN);
        args[0] = props;
        args[1] = NULL;
        Out("^y.Endpoint @^0\n", args);
        Iter ifcIt;
        Node *ifcs = Table_Get(props, K(m, "interfaces"));
        Iter_Init(&ifcIt, Span_Get(ifcs, INST_PROPIDX_CHILDREN));
        while((Iter_Next(&ifcIt) & END) == 0){
            Node *ifc = Iter_Get(&ifcIt);
            void *ar[] = {
                ifc,
                NULL
            };
            Out("^y.    Ifc @^0\n", ar);
        }
    }

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
