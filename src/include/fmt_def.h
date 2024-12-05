typedef Abstract *(*FmtTrans)(MemCtx *m, struct format_def *def, struct formatter *fmt, String *key, Abstract *a);

typedef struct format_def {
    Type type;
    int id;
    word _;
    String *name;
    String *alias;
    FmtTrans from;
    FmtTrans to;
    struct format_def *parent;
} FmtDef;

FmtDef *FmtDef_Make(MemCtx *m);
