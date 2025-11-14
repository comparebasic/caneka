#include <external.h>
#include <caneka.h>

status NodeObj_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();

    NodeObj *nobj = Inst_Make(m, TYPE_NODEOBJ);



    MemCh_Free(m);
    return r;
}
