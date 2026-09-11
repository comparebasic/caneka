typedef struct buildctx {
    Type type;
    MemCh *m;
    struct timespec start;
    struct timespec modified;
    StrVec *src;
    StrVec *dest;
    Ident *ident;
    Table *options;
    Table *deps /*<BuildModule>*/;
    Span *depsOrdered /*<BuildModule>*/;
    struct {
        Iter moduleIt /*<BuildModule>*/;;
        Iter sourcesIt /*<BuildModule>*/;;
        Span *flags;
    } current;
    struct {
        Str *cc;
        Str *ccVersion;
        Str *ar;
    } tools;
    struct {
        i32 sources;
        i32 built;
        i32 modules;
        i32 modulesBuilt;
    } metrics;
    CliStatus *cli;
    SourceFunc log;
} BuildCtx;


void BuildCtx_Config(BuildCtx *ctx);
status BuildCtx_Build(BuildCtx *ctx);
status BuildCtx_Log(BuildCtx *ctx);
status BuildCtx_SetFlags(BuildCtx *ctx);

BuildCtx *BuildCtx_Make(MemCh *m);
