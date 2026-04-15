#include <external.h>
#include <caneka.h>
#include <sysquery_api.h>

status SysQuery_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    NodeObj *sysdata = Inst_Make(m, TYPE_NODEOBJ);
    SysQuery(m, sysdata, ZERO);

    DebugStack_Pop();
    return r;
}
