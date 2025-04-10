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
        int count;
        int total;
        int modSrcCount;
        int modSrcTotal;
        int modCount;
        int modTotal;
        Str *modCount_s;
        Str *modTotal_s;
        Str *count_s;
        Str *total_s;
        Str *barStart; 
        Str *barEnd; 
    } steps;
    struct {
        Str *source; 
        Str *dest; 
        Str *action; 
    } current;
    Str *mem; 
    Str *mem_s; 
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
