#include <external.h>
#include <caneka.h>

status DocComment_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "body"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "refs"), I16_Wrapped(m, TYPE_TABLE));
    r |= Seel_Seel(m, tbl, S(m, "DocComment"), TYPE_DOC_COMMENT);
    return r;
}
