typedef void (*TaskFunc)(MemCh *m, Inst *tsk, struct serve *srv);
typedef Inst *(*TaskMaker)(MemCh *m, struct serve *srv);

typedef struct req_handler_def {
    Type type;
    ReqMaker mk;
    ReqFunc setup;
    ReqFunc handle;
    ReqFunc finalize;
    Iter routeIt; /*<Table>*/
    struct {
        ReqFunc open;
        ReqFunc final;
    } log;
    void *source;
} HandlerDef;

HandlerDef *HandlerDef_Make(MemCh *m);
