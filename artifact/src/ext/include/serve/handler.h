typedef struct handler {
    Type type;
    util id;
    SourceFunc func;
    Abstract *args;
    Span *depends;
} Handler;

Handler *Handler_Get(Handler *h);
