enum module_flags {
    BUILDMODULE_INC = 1 << 8,
    BUILDMODULE_SATISFIED = 1 << 9,
    BUILDMODULE_SOURCE_CHANGE = 1 << 10,
    BUILDMODULE_HEADER_CHANGE = 1 << 11,
    BUILDMODULE_UPSTREAM_CHANGE = 1 << 12,
};

typedef struct build_module {
    Type type;
    i32 idx;
    MemCh *m;
    StrVec *name;
    StrVec *target;
    StrVec *targetName;
    StrVec *src;
    StrVec *local;
    Table *execTbl;
    DirSel *sel;
    Span *flags;
    Span *libs;
    Node *config;
    struct timespec latest;
    struct  {
        i32 total;
        i32 built;
        i32 idx;
    } metrics;
    struct {
        Templ *src;
        Templ *dest;
        Templ *exec;
    } templ;
} BuildModule;

BuildModule *BuildModule_Make(MemCh *m, BuildCtx *ctx, StrVec *name);
BuildModule *BuildModule_FromIdent(MemCh *m, BuildCtx *ctx, Ident *ident);
void BuildModule_Load(BuildCtx *ctx, BuildModule *md);
void BuildModule_Gather(MemCh *m, BuildCtx *ctx, BuildModule *md);
void BuildModule_SetFlags(BuildCtx *ctx, BuildModule *md);
void BuildModule_Build(BuildCtx *ctx, BuildModule *md);
void BuildModule_BuildInc(BuildCtx *ctx, BuildModule *md);
