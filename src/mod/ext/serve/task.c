#include <external.h>
#include <caneka.h>

status Task_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "core"), I16_Wrapped(m, TYPE_TASK_CORE));
    Table_Set(tbl, S(m, "def"), I16_Wrapped(m, TYPE_HANDLER_DEF));
    Table_Set(tbl, S(m, "io"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_Set(tbl, S(m, "conn"), I16_Wrapped(m, TYPE_HOST_ENT));
    Table_Set(tbl, S(m, "capsule"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_Set(tbl, S(m, "domain"), I16_Wrapped(m, TYPE_WRAPPED_WORD));
    Table_Set(tbl, S(m, "protocol"), I16_Wrapped(m, TYPE_WRAPPED_WORD));
    Table_Set(tbl, S(m, "source"), I16_Wrapped(m, TYPE_ABSTRACT));
    r |= Seel_Seel(m, tbl, S(m, "Task"), TYPE_TASK);
    return r;
}
