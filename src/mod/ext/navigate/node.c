#include <external.h>
#include <caneka.h>

status Node_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_SetHashed(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    r |= Seel_Seel(m, tbl, S(m, "NodeObj"), TYPE_NODE);
    return r;
}
