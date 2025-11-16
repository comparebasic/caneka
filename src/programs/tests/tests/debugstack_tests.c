#include <external.h>
#include <caneka.h>

status DebugStack_Tests(MemCh *gm){
    status r = READY;

    MemCh *m = MemCh_Make();

    Buff *bf = Buff_Make(m, ZERO);
    Buff *bfAfter = Buff_Make(m, ZERO);

    DebugStack_Push(NULL, 0);
    DebugStack_Print(bf, 0);

    DebugStack_Pop();
    DebugStack_Print(bfAfter, 0);

    i64 total = bf->v->total;
    i64 totalAfter = bfAfter->v->total;
    r |= Test(total > totalAfter, "Stack printout has less characters than before", NULL); 
    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
