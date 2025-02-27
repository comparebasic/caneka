enum formatter_methods {
    FMT_UL = 1,
    FMT_TBL,
};

typedef struct formatter_ctx {
    Type type;
    MemCtx *m;
    Roebling *rbl;
    int indent;
    cls method;
    String *content;
    Lookup *methods;
    struct {
        File in;
        File out;
    } files;
} FmtHtml;

FmtCtx *FmtHtml_Make(MemCtx *m);
