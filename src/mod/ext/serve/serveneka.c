#include <external.h>
#include <caneka.h>

static Node *extStatic = NULL;
static Span *handlersStatic = NULL;

Abstract *EndPointKey_From(MemCh *m, Table *props){
    if(Table_Get(props, K(m, "ip4")) != NULL){
        StrVec *ip4Str = Table_Get(props, K(m, "ip4"));
        Single *port = Table_Get(props, K(m, "port"));
        NetAddr *addr = NetAddr_Make4(m);
        NetAddr_SetFromStr4(m, addr, Ifc(m, ip4Str, TYPE_STR));
        NetAddr_SetPort(m, addr, port->val.i);

        HostEnt *ent = HostEnt_Make(m, addr);

        return (Abstract *)ent;
    }else if(Table_Get(props, K(m, "ip6")) != NULL){
        printf("make ip6\n");
        fflush(stdout);
        return NULL;
    }else if(Table_Get(props, K(m, "file")) != NULL){
        StrVec *fname = Table_Get(props, K(m, "file"));
        Buff *bf = Buff_Make(m, ZERO);
        StrVec *fpath = IoUtil_AbsVec(m, fname);
        File_Open(bf, fpath, O_RDONLY|O_CREAT);
        Buff_PosEnd(bf);
        
        return(Abstract *)bf;
    }

    return NULL;
}

status HttpReq_Stats(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    return NOOP;
}

status Serveneka_Prepare(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    req->keys = Span_Make(m);
    Span_Add(req->keys, S(m, "by-path"));
    Span_Add(req->keys, hreq->path);

    void*ar[] = {
       hreq->path,
       NULL
    };

    if(Inst_GetByPath(req->def->ext, req->keys) != NULL){
        Req_StepHandled(m, req, srv);
        Req_SetRoute(m, req, srv);
    }else{
        Span_Wipe(req->keys);
        Span_Add(req->keys, S(m, "static"));
        Req_StepHandled(m, req, srv);
        Req_SetRoute(m, req, srv);
    }

    return req->type.state;
}

void Serveneka_Init(MemCh *m){
    if(extStatic == NULL){
        Crypto_Init(m);
        extStatic = Inst_Make(m, TYPE_NODE);

        Span *path = Span_Make(m);
        Span_Add(path, S(m, "static"));
        Inst_SetByPath(extStatic, path,
            Func_Wrapped(m, HttpStatic_RetrieveFile, INTERNAL_FLAGS));

        path = Span_Make(m);
        Span_Add(path, S(m, "by-path"));
        Span_Add(path, S(m, "/stats"));
        Inst_SetByPath(extStatic, path, 
            Func_Wrapped(m, HttpReq_Stats, INTERNAL_FLAGS));

        handlersStatic = Span_Make(m);
        Span_Add(handlersStatic, Func_Wrapped(m, Serveneka_Prepare, INTERNAL_FLAGS));

    }
}

void Serveneka_Serve(MemCh *m, Node *config){
    Debug_Push(m, config);
    void *args[5];

    Serveneka_Init(m);

    Span *path = Span_Make(m);
    Span_Add(path, K(m, "endpoints"));

    Serve *srv = Serve_Make(m);
    srv->config = config;

    Iter it;
    Inst_IterInitChild(&it, config, path);
    while((Iter_Next(&it) & END) == 0){
        Node *nd = Iter_Get(&it);
        Table *props = Span_Get(nd, INST_PROPIDX_CHILDREN);

        StrVec *handlerKey = Table_Get(props, K(m, "handler")); 
        Node *ext = NULL;
        Span *handlers = NULL;
        if(Equals(handlerKey, K(m, "http-static"))){
            ext = extStatic; 
            handlers = handlersStatic;
        }

        Iter ifcIt;
        Node *ifcs = Table_Get(props, K(m, "interfaces"));
        Iter_Init(&ifcIt, Span_Get(ifcs, INST_PROPIDX_CHILDREN));
        while((Iter_Next(&ifcIt) & END) == 0){
            Node *ifc = Iter_Get(&ifcIt);

            Table *ifcProps = Span_Get(ifc, INST_PROPIDX_CHILDREN);
            Abstract *key = EndPointKey_From(m, ifcProps);
            if(key == NULL){
                printf("Key is null, not supported\n");
                fflush(stdout);
                continue;
            }

            StrVec *proto = Table_Get(ifcProps, K(m, "proto"));
            DefMakerFunc func = HandlerDefFunc_ByVec(m, proto);
            HandlerDef *def = func(m, handlers, ext, key, ifc);
            def->subConfig = nd;

            if(key != NULL && def != NULL){
                Serve_AddEndpoint(srv, key, def);
            }
        }
    }

    Serve_Serve(srv);
}
