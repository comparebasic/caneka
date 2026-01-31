#include <external.h>
#include <caneka.h>

status WwwPage_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "nav"), I16_Wrapped(m, TYPE_WWW_NAV));
    Table_Set(tbl, S(m, "coords"), I16_Wrapped(m, TYPE_SPAN));
    Table_Set(tbl, S(m, "url"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "header"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_Set(tbl, S(m, "footer"), I16_Wrapped(m, TYPE_ABSTRACT));
    r |= Seel_Seel(m, tbl, S(m, "WwwPage"), TYPE_WWW_PAGE);
    return r;
}
