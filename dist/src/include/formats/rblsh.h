typedef struct rblsh_ctx {
    Type type;
    int idx;
    struct {
        Span *p;
        String *s;
    } cwd;
    String *shelf;
    Span *env;
    Span *path;
    struct cash *cash;
} RblShCtx;

Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s);
