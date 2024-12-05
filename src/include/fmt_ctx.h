enum formatter_flags {
    FMT_FL_TO_PARENT = 1 << 8,
    FMT_FL_TO_PARENT_ON_PARENT = 1 << 9,
    FMT_FL_TO_NEXT_ID = 1 << 10,
    FMT_FL_AUTO_ID = 1 << 11,
};
typedef struct fmt_item {
    Type type;
    word flags;
    FmtDef *def;
    int offset;
    String *content;
    String *key;
    Abstract *value;
    int remaining;
    struct fmt_item *parent;
} FmtItem;

typedef struct formatter {
    Type type;
    MemCtx *m;
    /* current item */
    FmtItem *item;
    int id;
    word offset;
    /* internal */
    Roebling *rbl;
    Chain *byId;
    TableChain *byName;
} FmtCtx;

FmtItem *FmtItem_Make(MemCtx *m, FmtCtx *fctx);
FmtCtx *FmtCtx_Make(MemCtx *m);
status Fmt_Add(MemCtx *m, FmtCtx *fmt, Lookup *fmtDefs);
