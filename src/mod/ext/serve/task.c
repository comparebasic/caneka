#include <external.h>
#include <caneka.h>

status Task_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "core"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_Set(tbl, S(m, "io"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_SetHashed(tbl, S(m, "capsule"), I16_Wrapped(m, TYPE_ABSTRACT));
    return r;
}
