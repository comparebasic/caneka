typedef void (*ReqFunc)(MemCh *m, struct req *req, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    SourceMakerFunc extra;
    ReqFunc finalize;
    Span *route;
    Node *tbl;
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
    CapsuleDef *capsule;
    void *source;
} HandlerDef;

HandlerDef *HandlerDef_Make(MemCh *m);
