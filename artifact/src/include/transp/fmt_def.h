typedef Abstract *(*FmtTrans)(MemCtx *m, struct format_def *def, struct fmt_ctx *fmt, String *key, Abstract *a);

enum formatter_def_flags {
    FMT_DEF_INDENT = 1 << 8,
    /*FMT_FL_TAXONAMY = 1 << 9,*/
    FMT_DEF_OUTDENT = 1 << 10,
    FMT_DEF_PARENT_ON_PARENT = 1 << 11,
    FMT_DEF_TO_ON_CLOSE = 1 << 12,
};

typedef struct format_def {
    Type type;
    int id;
    int flags;
    String *name;
    String *alias;
    FmtTrans from;
    FmtTrans to;
    struct format_def *parent;
} FmtDef;

FmtDef *FmtDef_Make(MemCtx *m);
void FmtDef_PushItem(struct fmt_ctx *ctx, word captureKey, String *s, FmtDef *def);
