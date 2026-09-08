enum module_flags {
    BUILDMODULE_INC = 1 << 8,
    BUILDMODULE_SATISFIED = 1 << 9,
};

typedef struct build_module {
    Type type;
    StrVec *name;
    StrVec *target;
    StrVec *targetName;
    StrVec *src;
    DirSel *sel;
    Node *config;
    struct timespec latest;
    struct  {
        i32 sources;
        i32 built;
        i32 idx;
    } metrics;
} BuildModule;

BuildModule *BuildModule_Make(MemCh *m, BuildCtx *ctx, StrVec *name);
BuildModule *BuildModule_FromIdent(MemCh *m, BuildCtx *ctx, Ident *ident);
void BuildModule_Load(BuildCtx *ctx, BuildModule *md);
void BuildModule_Gather(MemCh *m, BuildCtx *ctx, BuildModule *md);
