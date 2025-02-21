typedef struct target {
    char *bin;
    char *src;
} Target;

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;

typedef struct buildctx {
    Type type;
    MemCtx *m;
    Target *targets;
    char *dist;
    char *src;
    struct {
        char *cc;
        char *ar;
    } tools;
    struct {
        char **inc;
        char **cflags;
        char **libs;
    } args;
    BuildSubdir **objdirs;
} BuildCtx;

static int BuildSource(BuildCtx *ctx, char *binary, char *fname, char *subdir);
static int BuildTarget(BuildCtx *ctx, char *binary, char *fname);
