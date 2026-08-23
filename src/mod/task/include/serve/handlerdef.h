typedef void (*ReqFunc)(MemCh *m, struct req *req, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    SourceMakerFunc extra;
    DoFunc setup;
    ReqFunc finalize;
    Span *route;
    Node *ext;
    struct host_ent *ent;
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
    CapsuleDef *capsule;
    Node *subConfig;
    void *source;
} HandlerDef;

typedef HandlerDef *(*DefMakerFunc)(MemCh *m,
    Span *handlers, Node *extensions, Abstract *key, Node *config);

DefMakerFunc HandlerDefFunc_ByVec(MemCh *m, StrVec *proto);
HandlerDef *HandlerDef_Make(MemCh *m);
