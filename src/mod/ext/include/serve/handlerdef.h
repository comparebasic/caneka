typedef void (*ReqFunc)(MemCh *m, struct req *req, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    Maker mk;
    ReqFunc setup;
    ReqFunc handle;
    ReqFunc finalize;
    Iter routesIt; /*<Table>*/
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
} HandlerDef;

HandlerDef *HandlerDef_Make(MemCh *m);
