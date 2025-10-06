enum handler_flags {
    HANDLER_QUEUE = 1 << 8,
};

typedef struct handler {
    Type type;
    util id;
    SourceMakerFunc func;
    Abstract *arg;
    struct handler *depends;
} Handler;

Handler *Handler_Get(Handler *h, Handler *chain);
Handler *Handler_Make(MemCh *m, SourceMakerFunc func, Abstract *arg, Abstract *source);
