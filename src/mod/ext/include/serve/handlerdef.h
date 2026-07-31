typedef void (*ReqFunc)(MemCh *m, struct req *req, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    SourceMakerFunc extra;
    ReqFunc finalize;
    Span *route;
    Node *extensions;
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
    CapsuleDef *capsule;
    void *source;
} HandlerDef;

typedef HandlerDef *(*DefMakerFunc)(MemCh *m, Span *handlers, Node *extensions);
HandlerDef *HandlerDef_Make(MemCh *m);
void HandlerDef_SetExt(MemCh *m, struct req *req, Span *path);
