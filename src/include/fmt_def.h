typedef Abstract *(*FmtTrans)(MemCtx *m, struct format_def *def, struct formatter *fmt, String *key, Abstract *a);

typedef struct format_def {
    Type type;
    int id;
    word _;
    String *name;
    FmtTrans from;
    FmtTrans to;
} FmtDef;

FmtDef *FmtDef_Make(MemCtx *m);
