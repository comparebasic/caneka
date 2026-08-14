typedef struct shell_req {
    Type type;
    MemCh *m;
    Buff *in;
    Buff *out;
    Buff *err;
    StrVec *shelf;
    Table *env;
    Table *args;
    Table *procs;
    void *source;
} ShellReq;

void *ShellReq_SourceMake(MemCh *m, Abstract *key, HandlerDef *def);
HandlerDef *ShellReq_DefMake(MemCh *m,
    Span *steps, Node *ext, Abstract *key, Node *config);
