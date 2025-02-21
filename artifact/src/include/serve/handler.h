typedef status (*HandleFunc)(struct handler *h, struct req *req, struct serve_ctx *sctx);

typedef struct handler {
    Type type;
    HandleFunc func;
    int direction;
    int id;
    Abstract *data;
    Span *prior;
} Handler;

Handler *Handler_Get(Handler *h);
Handler *Handler_Make(MemCtx *m, HandleFunc func, Abstract *data, int direction, int id);
status Handler_AddPrior(MemCtx *m, Handler *orig, Handler *h);
