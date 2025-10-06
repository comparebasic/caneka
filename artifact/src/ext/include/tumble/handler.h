enum handler_flags {
    HANDLER_QUEUE = 1 << 8,
};

typedef status (*HandlerFunc)(struct handler *chain);

typedef struct handler {
    Type type;
    util id;
    HandlerFunc func;
    Abstract *arg;
    Abstract *source;
    struct handler *depends;
} Handler;

typedef Handler *(*HandlerMaker)(MemCh *m,
    Handler *chain, Abstract *arg, Abstract *source);

Handler *Handler_Get(Handler *h, Handler *chain);
Handler *Handler_Make(MemCh *m, HandlerFunc func, Abstract *arg, Abstract *source);
Abstract *Handler_Add(MemCh *m, Handler *chain, HandlerFunc func, Abstract *arg, Abstract *source);
