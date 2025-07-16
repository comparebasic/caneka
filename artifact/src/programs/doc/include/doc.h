enum doc_types {
    _DOC_TYPES_START = _TYPE_APPS_END,
    TYPE_DOC_CTX,
};

typedef struct doc_ctx {
    Type type;
    MemCh *m;
    Table *args;
    Nested *nav;
    OrdTable *pages;
    OrdTable *modules;
    Str *fmtPath;
} DocCtx;

DocCtx *DocCtx_Make(MemCh *m);

Table *DocCtx_ArgResolve(DocCtx *ctx, int argc, char *argv[]);
status DocCtx_SetFmtPath(DocCtx *ctx, Str *path);
