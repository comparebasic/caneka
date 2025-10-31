#include <external.h>
#include <caneka.h>

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "./examples/web-server/pages/public"));
    args[0] = (Abstract *)path;
    args[1] = NULL;
    Out("^p.Path: @\n^0", args);

    Route_Collect(rt, path);

    args[0] = (Abstract *)rt;
    args[1] = NULL;
    Out("^p.Route @\n^0", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
