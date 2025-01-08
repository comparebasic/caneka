typedef struct rblsh_ctx {
    Type type;
    int idx;
    struct {
	Span *p;
	String *s;
    } cwd;
    Span *env;
    Span *path;
} RblShCtx;

Roebling *RblShParser_Make(MemCtx *m, RblShCtx *ctx, String *s, RblCaptureFunc capture);
