typedef Abstract *(*FmtTrans)(MemCtx *m, struct format_def *def, struct formatter *fmt, String *key, Abstract *a);

enum formatter_def_flags {
    FMT_DEF_INDENT = 1 << 8,
    /*FMT_FL_TAXONAMY = 1 << 9,*/
    FMT_DEF_OUTDENT = 1 << 10,
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
