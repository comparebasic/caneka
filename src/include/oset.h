enum oset_states {
    OSET_START = 0,
    OSET_KEY = 0,
    OSET_LENGTH = 0,
    OSET_OPTS = 0,
    OSET_VALUE = 0
};

typedef oset {
    Type type;
    String *content;
    Chain *defs;
    /* internal */
    int remaining;
    cls currentType;
    Roebling *rbl;
    Lookup *byType;
    TableChain *byName;
} Oset;

typedef status (*ToOset)(MemCtx *m, OsetCtx *oset, Abstract *a);

typedef oset_def {
    Type type;
    cls typeOf;
    word _;
    String *name;
    Maker fromOset;
    ToOset toOset;
} OsetDef;

Oset *Oset_Make(MemCtx *m, Lookup *osetDefs);
status Oset_Add(MemCtx *m, Oset *o, Lookup *osetDefs);

OsetDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, Maker from, ToOset to);

String *Oset_To(MemCtx *m, Abstract *a);
Abstract *Abs_FromOset(MemCtx *m, String *s);
