#include <external.h>
#include <caneka.h>

void Serveneka_Serve(MemCh *m, Node *config){
    Debug_Push(m, config);
    void *args[5];

    args[0] = config;
    args[1] = NULL;
    Out("^p.Config: $^0\n", args);

    Node *endpoints = Inst_GetByPath(config, Sv(m, "endpoints"));

    Iter it;
    Inst_IterInitChild(&it, config, Sv(m, "endpoints"));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Table *props = Span_Get(h->value, INST_PROPIDX_CHILDREN);
            StrVec *type = Table_Get(props, K(m, "type"));
            StrVec *handler = Table_Get(props, K(m, "handler"));
            if(type == NULL || handler == NULL){
                args[0] = props;
                args[1] = NULL;
                Out("^Error Configuring Endpoint type or handler is NULL $", args);
                ReturnVoid(m);
            }

            if(Equals(type, K(m, "ip4"))){
                /* parse key */
                /* make HostEnt as key to routes Hashed */
                /* grap handlers and make value to routes Hashed */
                HostEnt *ent = HostEnt_Make(m);
                NetAddr *addr = NetAddr_Make4(m); 
                ent->addr = addr;
            }else if(Equals(type, K(m, "ip6"))){
                /*
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Not yet implemented", NULL);
                    */
            if(Equals(type, K(m, "file"))){
                args[0] = props;
                args[1] = NULL;
                Out("^y.File $^0\n", args);
            }else{
                Buff *bf = Buff_Make(m, ZERO);
            }
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
