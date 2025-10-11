#include <external.h>
#include <caneka.h>

status FileD_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
