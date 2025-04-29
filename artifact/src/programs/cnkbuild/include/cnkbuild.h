typedef struct executable {
    char *bin;
    char *src;
} Executable;

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;

typedef struct build_cli_fields {
    struct {
        Str *name; 
        Single *count;
        Single *total;
        Single *modSrcCount;
        Single *modSrcTotal;
        Single *modCount;
        Single *modTotal;
        Str *barStart; 
        Str *barEnd; 
    } steps;
    struct {
        Str *source; 
        Str *dest; 
        Str *action; 
    } current;
} BuildCliFields;

typedef struct buildctx {
    Type type;
    MemCh *m;
    Executable *targets;
    CliStatus *cli;
    BuildCliFields fields;
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

status BuildCtx_Init(MemCh *m, BuildCtx *ctx);
status Build(BuildCtx *ctx);
