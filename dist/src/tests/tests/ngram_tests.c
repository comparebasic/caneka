#include <external.h>
#include <caneka.h>

status MemLocal_Tests(MemCtx *gm){
    DebugStack_Push("MemLocal_Test", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    MemCtx_Free(m);

    DebugStack_Pop();
    return r;
}
