#include <external.h>
#include <caneka.h>

Table *extensions = NULL;

status HttpReq_Stats(MemCh *m, Req *req, Serve *srv){
    return NOOP;
}

status ServenekaReq_Prepare(MemCh *m, Req *req, Serve *srv){
    Span *path = Span_Make(m);
    Span_Add(path, K(m, "by-path"));
    Span_Add(path, req->path);
    Single *sg = Inst_ByPath(req->def->extensions, path);
    if(sg != NULL){
        Span_Set(req->chain.p, req->chain.idx, sg);
    }else{
        Span *path = Span_Make(m);
        Span_Add(path, K(m, "static"));
        sg = Inst_ByPath(req->def->extensions, path);
    }

    if(sg != NULL){
        ReqFunc func = (ReqFunc)sg->val.ptr;
        func(m, req, srv);
    }
    
    return req->type.state;
}

void Serveneka_Serve(MemCh *m, Node *config){
    Debug_Push(m, config);
    void *args[5];

    args[0] = config;
    args[1] = NULL;
    Out("^p.Config: $^0\n", args);


    Span *path;

    Node *extensions = Inst_Make(m, TYPE_NODE);

    path = Span_Make(m);
    Span_Add(path, S(m, "static"));
    Inst_SetByPath(extensions, path, Func_Wrapped(m, HttpStatic_RetrieveFile));

    path = Span_Make(m);
    Span_Add(path, S(m, "by-path"));
    Span_Add(path, S(m, "/stat"));
    Inst_SetByPath(extensions, path, Func_Wrapped(m, HttpReq_Stats));

    path = Span_Make(m);
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

void Serveneka_Init(MemCh *m, Node *config){
    if(extensions == NULL){
        extensions = Table_Make(m);
        Table_Set(extensions, S(m, "static"),
            );
        Table_Set(extensions, S(m, "/stats"), 
            Func_Wrapped(m, HttpReq_Stats));
    }
}
