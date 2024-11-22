typedef struct oset_item {
    Type type;
    String *content;
    String *key;
    Abstract *value;
    word flags;
    struct oset_def *odef;
    int remaining;
    struct oset_item *parent;
} OsetItem;

Abstract *Abs_FromOsetItem(MemCtx *m, struct oset *o, OsetItem *oi);
OsetItem *OsetItem_Make(MemCtx *m, struct oset *o);
