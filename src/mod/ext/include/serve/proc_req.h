typedef struct proc_req {
    Type type;
    MemCh *m;
    Buff *in;
    Buff *out;
    Buff *err;
    Span *cmd;
    ProcDets *pd;
    void *source;
} ProcReq;

void *ProcReq_SourceMake(MemCh *m, Abstract *key, HandlerDef *def);
HandlerDef *ProcReq_DefMake(MemCh *m,
    Span *handlers, Node *ext, Abstract *key, Node *config);
