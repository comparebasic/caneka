#include <external.h>
#include <caneka.h>

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "examples/web-server/pages/public"));
    Route_Collect(rt, path);

    Abstract *args[] = {
        (Abstract *)rt,
        NULL
    };
    Out("^p.Route @\n^0", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
