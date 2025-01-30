enum rblsh_type_range {
    _RBLSH_START = _TYPE_APPS_END,
    TYPE_RBLSH_SUPER,
    TYPE_RBLSH_CTX,
    RBLSH_MARK_START,
    RBLSH_MARK_END,
    RBLSH_TERM,
    RBLSH_HUP,
    RBLSH_WS,
    RBLSH_NL,
    RBLSH_ARG,
    RBLSH_FLAG,
    RBLSH_WORDFLAG,
    RBLSH_OP,
    RBLSH_STRING_LIT,
    RBLSH_STRING_OP,
    _RBLSH_END,
};

typedef struct rblsh_super {
    Type type;
    Req *req;
} RblShSuper;

typedef struct rblsh_driver {
    Type type;
    Req *req;
} RblShDriver;

typedef struct rblsh_ctx {
    Type type;
    int idx;
    Serve *sctx;
    RblShSuper *current;
    Span *supers;
    struct {
        Span *p;
        String *s;
    } cwd;
    struct {
    Roebling *rbl;
    Span *env;
    Span *path;
    struct cash *cash;
} RblShCtx;

/* parser */
Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s);

/* Super */
RblShSuper *RblShSuper_Make(MemCtx *m, RblShCtx *ctx);
status RblShSuper_Capture(word rkey, int matchIdx, String *s, Abstract *source);

/* req */
Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx);
Handler *RblSh_GetHandler(Serve *sctx, Req *req);

/* ctx */
Abstract *RblShCtx_GetVar(Abstract *store, Abstract *key);
RblShCtx *RblShCtx_Make(MemCtx *m);
status RblShDebug_Init(MemCtx *m);
status RblShCtx_Capture(word rkey, int matchIdx, String *s, Abstract *source);
status RblShCtx_RunCmd(RblShCtx *ctx);
