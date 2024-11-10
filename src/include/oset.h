enum oset_states {
    OSET_START = 0,
    OSET_KEY,
    OSET_TOKEN,
    OSET_LENGTH,
    OSET_OPTS,
    OSET_VALUE,
};

enum oset_marks {
    OSET_MARK_START = 1,
    OSET_MARK_LENGTH,
};

enum oset_flags {
    LINE_SEPERATED = 1 << 10,
};

typedef struct oset {
    Type type;
    MemCtx *m;
    /* current item */
    String *content;
    String *key;
    struct oset_def *odef;
    int remaining;
    /* internal */
    Roebling *rbl;
    Chain *byType;
    TableChain *byName;
} Oset;

typedef Abstract *(*OsetTrans)(MemCtx *m, struct oset_def *odef, struct oset *oset, String *key, Abstract *a);

typedef struct oset_def {
    Type type;
    cls typeOf;
    word _;
    String *name;
    OsetTrans fromOset;
    OsetTrans toOset;
} OsetDef;

status Oset_Init(MemCtx *m);
Oset *Oset_Make(MemCtx *m, Lookup *osetDefs);
status Oset_Add(MemCtx *m, Oset *o, Lookup *osetDefs);

OsetDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, OsetTrans from, OsetTrans to);

String *Oset_To(MemCtx *m, String *key, Abstract *a);
Abstract *Abs_FromOset(MemCtx *m, String *s);
Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source);

#include "formats/oset_to.h"
#include "formats/oset_from.h"
