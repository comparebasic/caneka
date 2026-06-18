typedef struct req_handler_def {
    Type type;
    Req_Mk mk;
    Req_Setup setup;
    SourceFunc handle;
    SourceFunc finalize;
    Iter routesIt; /*<Table>*/
    struct {
        SourceFunc open;
        SourceFunc final;
    } log;
} HandlerDef;

HandlerDef *HandlerDef_Make(MemCh *m);
