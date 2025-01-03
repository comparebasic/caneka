enum format_item_flags {
    FMT_FL_TO_PARENT = 1 << 8,
    FMT_FL_TAXONAMY = 1 << 9,
    FMT_FL_TO_NEXT_ID = 1 << 10,
    FMT_FL_TO_PARENT_ON_LAST = 1 << 11,
    FMT_FL_TO_NEXT_ON_LAST = 1 << 12,
    FMT_FL_CLOSE_AFTER_CHILD = 1 << 13,
    FMT_FL_AUTO_ID = 1 << 14,
    FMT_FL_SPACE_FOR_WS = 1 << 15,
};

enum format_ctx_flags {
    FMT_CTX_ENCOUNTER = 1 << 8,
    FMT_CTX_CHILDREN_DONE = 1 << 9,
};

typedef struct fmt_cnf  {
    word id;
    word state;
    int flags;
    char *name;
    char *alias;
    FmtTrans to;
    word *rollups;
    int parentIdx;
} FmtCnf;

typedef struct fmt_item {
    Type type;
    int flags;
    int subId;
    FmtDef *def;
    word spaceIdx;
    int offset;
    String *content;
    String *key;
    Abstract *value;
    Span *children;
    int remaining;
    struct fmt_item *parent;
} FmtItem;


typedef struct formatter {
    Type type;
    MemCtx *m;
    FmtDef *def;
    FmtItem *item;
    FmtItem *root;
    word offset;
    Roebling *rbl;
    Chain *byId;
    TableChain *byName;
    TableChain *byAlias;
    RangeToChars rangeToChars;
    OutFunc out;
    status (*start)(struct formatter *ctx);
    Abstract *source;
} FmtCtx;

FmtItem *FmtItem_Make(MemCtx *m, FmtCtx *fctx);
Span *FmtItem_GetChildren(MemCtx *m, FmtItem *fi);
FmtCtx *FmtCtx_Make(MemCtx *m, OutFunc out);
status Fmt_Add(MemCtx *m, FmtCtx *fmt, Lookup *fmtDefs);
