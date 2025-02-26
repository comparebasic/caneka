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
        StrVecEntry *name; 
        int count;
        int total;
        String *count_s;
        String *total_s;
        StrVecEntry *count_ve; 
        StrVecEntry *total_ve; 
        StrVecEntry *barStart; 
        StrVecEntry *barEnd; 
    } steps;
    struct {
        StrVecEntry *source; 
        StrVecEntry *dest; 
        StrVecEntry *action; 
    } current;
    StrVecEntry *mem; 
    String *mem_s; 
} BuildCliFields;

typedef struct buildctx {
    Type type;
    MemCtx *m;
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

status BuildCtx_Init(MemCtx *m, BuildCtx *ctx);
status Build(BuildCtx *ctx);
