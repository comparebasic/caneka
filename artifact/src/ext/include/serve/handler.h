typedef struct handler {
    Type type;
    util id;
    SourceMakerFunc func;
    Abstract *arg;
    struct handler *depends;
} Handler;

Handler *Handler_Get(Handler *h, Handler *chain);
Handler *Handler_Make(MemCh *m, SourceFunc func, Abstract *arg);
