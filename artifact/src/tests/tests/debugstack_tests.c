#include <external.h>
#include <caneka.h>

status DebugStack_Tests(MemCh *gm){
    status r = READY;

    MemCh *m = MemCh_Make();
    Stream *sm = Stream_MakeStrVec(m);
    Stream *smAfter = Stream_MakeStrVec(m);

    DebugStack_Push(NULL, 0);
    DebugStack_Print(sm, 0);
    DebugStack_Pop();
    DebugStack_Print(smAfter, 0);

    i64 total = sm->dest.curs->v->total;
    i64 totalAfter = smAfter->dest.curs->v->total;
    Abstract *args[] = {
        (Abstract *)sm->dest.curs->v,
        (Abstract *)smAfter->dest.curs->v,
        NULL
    };
    r |= Test(total > totalAfter, 
        "Stack printout has less characters than before, stack: @\nstackAfter: @\n", args); 
    MemCh_Free(m);

    DebugStack_Pop();
    return r;
}
