typedef struct fmt_def_resolver {
    Type type;
    Chain *byId;
    TableChain *byName;
    TableChain *byAlias;
} FmtResolver;

FmtResolver *FmtResolver_Make(MemCtx *m);
