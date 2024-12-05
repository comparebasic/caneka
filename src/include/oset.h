enum oset_states {
    OSET_START = 0,
    OSET_KEY,
    OSET_TOKEN,
    OSET_FLAG,
    OSET_LENGTH,
    OSET_LENGTH_LIST,
    OSET_LENGTH_TABLE,
    OSET_LENGTH_ARRAY,
    OSET_OPTS,
    OSET_VALUE,
    OSET_SEP,
    OSET_CLOSE_LIST,
    OSET_CLOSE_ARRAY,
    OSET_CLOSE_TABLE,
};

enum oset_marks {
    OSET_MARK_START = 1,
    OSET_MARK_LENGTH,
};

enum oset_flags {
    LINE_SEPERATED = 1 << 10,
    PARENT_TYPE_ARRAY = 1 << 12,
    PARENT_TYPE_TABLE = 1 << 13,
};

status Oset_Init(MemCtx *m);
FmtCtx *Oset_Make(MemCtx *m, Lookup *osetDefs);
status Oset_Add(MemCtx *m, FmtCtx *o, Lookup *osetDefs);

FmtDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, FmtTrans from, FmtTrans to);

String *Oset_To(MemCtx *m, String *key, Abstract *a);
Abstract *Abs_FromOset(MemCtx *m, String *s);
Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source);

#include "formats/oset_to.h"
#include "formats/oset_from.h"
