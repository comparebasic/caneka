#include <external.h>
#include "base_module.h"

Str *State_ToStr(MemCh *m, status state){
    StrVec *v = Type_StateVec(m, ZERO, state);
    return StrVec_Str(m, v);
}
