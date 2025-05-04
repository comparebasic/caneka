#include <external.h>
#include <caneka.h>

Str *State_ToStr(MemCh *m, status state){
    Str *s = Str_Make(m, STR_DEFAULT);
    Str_AddFlagLabels(s, state, stateLabels);
    return s;
}
