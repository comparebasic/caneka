#include <external.h>
#include <caneka.h>

static Node *httpStaticExt = NULL;
static Span *handlers = NULL;

status HttpReq_Stats(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    return NOOP;
}

status ServenekaReq_Handle(MemCh *m, Req *req, Serve *srv){
    HttpReq *hreq = (HttpReq *)req->source;
    Span *path = Span_Make(m);
    Span_Add(path, K(m, "by-path"));
    Span_Add(path, hreq->path);

    HandlerDef_SetExt(m, req, path);
    if((req->type.state & MORE) == 0){
        path = Span_Make(m);
        Span_Add(path, K(m, "static"));
        HandlerDef_SetExt(m, req, path);
    }

    return req->type.state;
}

void Serveneka_Serve(MemCh *m, Node *config){
    Debug_Push(m, config);
    void *args[5];

    Table *routes = Table_Make(m);

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
        Span *handlers = Span_Make(m);
        args[0] = props;
        args[1] = NULL;
        Out("^y.Endpoint @^0\n", args);
        StrVec *handlerKey = Table_Get(props, K(m, "handler")); Node *ext = NULL; if(Equals(handlerKey, K(m, "http-static"))){
            ext = httpStaticExt; 
        }

        Iter ifcIt;
        Node *ifcs = Table_Get(props, K(m, "interfaces"));
        Iter_Init(&ifcIt, Span_Get(ifcs, INST_PROPIDX_CHILDREN));
        while((Iter_Next(&ifcIt) & END) == 0){
            Node *ifc = Iter_Get(&ifcIt);

            Abstract *key = NULL;
            HandlerDef *def = NULL;

            Table *ifcProps = Span_Get(ifc, INST_PROPIDX_CHILDREN);
            StrVec *proto = Table_Get(ifcProps, K(m, "proto"));
            StrVec *ip4Str = Table_Get(ifcProps, K(m, "ip4"));
            StrVec *ip6Str = Table_Get(ifcProps, K(m, "ip6"));
            Single *port = Table_Get(ifcProps, K(m, "port"));
            StrVec *fname = Table_Get(ifcProps, K(m, "file"));

            if(ip4Str != NULL){
                NetAddr *addr = NetAddr_Make4(m);
                NetAddr_SetFromStr4(m, addr, Ifc(m, ip4Str, TYPE_STR));
                NetAddr_SetPort(m, addr, port->val.i);

                HostEnt *ent = HostEnt_Make(m, addr);

                key = (Abstract *)ent;
                Single *sg = Table_Get(ServeProtoTable, proto);
                if(sg == NULL){
                    void *ar[] = {proto, NULL};
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Error proto not found @^0\n", ar);
                   ReturnVoid(m);
                }

                DefMakerFunc func = (DefMakerFunc)sg->val.ptr;
                def = func(m, handlers, httpStaticExt);
                def->source = ent;
            }else if(ip6Str != NULL){
                printf("make ip6\n");
                fflush(stdout);
            }else if(fname){
                Buff *bf = Buff_Make(m, ZERO);
                StrVec *fpath = IoUtil_AbsVec(m, fname);
                File_Open(bf, fpath, O_RDONLY|O_CREAT);
                Buff_PosEnd(bf);
                key = (Abstract *)bf;
            }

            if(key != NULL && def != NULL){
                Table_Set(routes, key, def);
            }

            void *ar[] = {
                ifc,
                key,
                def,
                NULL
            };
            Out("^y.    Ifc @\n  @ -> @\n^0\n", ar);
        }
    }

    Serve *srv = Serve_Make(m, routes, config);
    Serve_Serve(srv);
}

void Serveneka_Init(MemCh *m, Node *config){
    if(httpStaticExt == NULL){
        httpStaticExt = Inst_Make(m, TYPE_NODE);
        Span *path = Span_Make(m);
        Span_Add(path, S(m, "static"));
        Inst_SetByPath(httpStaticExt, path, Func_Wrapped(m, HttpStatic_RetrieveFile));

        path = Span_Make(m);
        Span_Add(path, S(m, "by-path"));
        Span_Add(path, S(m, "/stats"));
        Inst_SetByPath(httpStaticExt, path, Func_Wrapped(m, HttpReq_Stats));

        handlers = Span_Make(m);
        Span_Add(handlers, Func_Wrapped(m, ServenekaReq_Handle));
    }
}
