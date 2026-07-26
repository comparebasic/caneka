typedef void (*ReqFunc)(MemCh *m, struct req *req, struct serve *srv);
typedef struct req *(*ReqMaker)(MemCh *m, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    ReqMaker mk;
    ReqFunc setup;
    ReqFunc handle;
    ReqFunc finalize;
    Iter routeIt;
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
    CapsuleDef *capsule;
    void *source;
} HandlerDef;

HandlerDef *HandlerDef_Make(MemCh *m);
