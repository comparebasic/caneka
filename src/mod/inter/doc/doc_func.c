#include <external.h>
#include <caneka.h>

status DocFunc_Init(MemCh *m){
    status r = READY;

    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "ret"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "args"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "comments"), I16_Wrapped(m, TYPE_SPAN));
    Table_Set(tbl, S(m, "body"), I16_Wrapped(m, TYPE_STRVEC));
    r |= Seel_Seel(m, tbl, S(m, "DocFunc"), TYPE_DOC_FUNC);

    return r;
}
