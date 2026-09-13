typedef struct buildctx {
    Type type;
    MemCh *m;
    struct timespec start;
    struct timespec modified;
    StrVec *src;
    StrVec *dest;
    Ident *ident;
    Table *options;
    Table *optionNames;
    Table *deps /*<BuildModule>*/;
    Span *depsOrdered /*<BuildModule>*/;
    struct {
        Iter moduleIt /*<BuildModule>*/;;
        Iter sourcesIt /*<BuildModule>*/;;
        Span *flags;
        Span *statLibs;
        Span *libs;
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
status BuildCtx_Setup(BuildCtx *ctx);
status BuildCtx_SetStatus(BuildCtx *ctx);
status BuildCtx_ShowStatus(BuildCtx *ctx);
status BuildCtx_Build(BuildCtx *ctx);
status BuildCtx_Log(BuildCtx *ctx);
status BuildCtx_SetFlags(BuildCtx *ctx);
status BuildCtx_MakeInclude(BuildCtx *ctx);

BuildCtx *BuildCtx_Make(MemCh *m);
