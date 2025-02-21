typedef struct executable {
    char *bin;
    char *src;
} Executable;

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;

typedef struct buildctx {
    Type type;
    MemCtx *m;
    Executable *targets;
    char *dist;
    char *src;
    char *libtarget;
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

status BuildCtx_Init(BuildCtx *ctx);
status Build(BuildCtx *ctx);
