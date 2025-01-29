#define INREQ SERVE_ALPHA 
#define OUTREQ SERVE_BRAVO 
#define ERRREQ SERVE_CHARLIE 

enum rblsh_type_range {
    _RBLSH_START = _TYPE_APPS_END,
    TYPE_RBLSH_SUPER,
};

struct rblsh_super {
    Type type;
    Req *req;
} RblShSuper;



/* Super */
RblShSuper *RblShSuper_Make(MemCtx *m, Req *req);

/* req */
Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx);

/* rblctx */
Abstract *RblShCtx_GetVar(Abstract *store, Abstract *key);
RblShCtx *RblShCtx_Make(MemCtx *m);
status RblShDebug_Init(MemCtx *m);
