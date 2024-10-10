typedef status (*HandleFunc)(struct handler *h, struct serve_req *req, struct serve_ctx *sctx);

typedef struct handler {
    Type type;
    HandleFunc func;
    int direction;
    Abstract *data;
    Span *prior;
} Handler;

Handler *Handler_Get(Handler *h);
Handler *Handler_Current(Handler *h);
Handler *Handler_Make(MemCtx *m, HandleFunc func, Abstract *data);
