enum oset_marks {
    OSET_MARK_START,
    OSET_MARK_LENGTH,
    _OSET_MARK_MAX = 1 << 15
};

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
    OSET_LABEL,
    OSET_TYPE,
};

enum oset_flags {
    LINE_SEPERATED = 1 << 10,
    PARENT_TYPE_ARRAY = 1 << 12,
    PARENT_TYPE_TABLE = 1 << 13,
};

status Oset_Init(MemCtx *m);
FmtCtx *Oset_Make(MemCtx *m, Lookup *osetDefs);

FmtDef *OsetDef_Make(MemCtx *m, cls typeOf, String *name, FmtTrans from, FmtTrans to);

String *Oset_To(MemCtx *m, String *key, Abstract *a);
Abstract *Abs_FromOset(MemCtx *m, String *s);
Roebling *OsetParser_Make(MemCtx *m, String *s, Abstract *source);

#include "formats/oset_to.h"
#include "formats/oset_from.h"
