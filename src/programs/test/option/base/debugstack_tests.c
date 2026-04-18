#include <external.h>
#include <caneka.h>
#include <test_module.h>

status DebugStack_Tests(MemCh *m){
    status r = READY;

    Buff *bf = Buff_Make(m, ZERO);
    Buff *bfAfter = Buff_Make(m, ZERO);

    Debug_Push(m, NULL);
    DebugStack_Print(m, bf, 0);

    DebugStack_Pop(m);
    DebugStack_Print(m, bfAfter, 0);

    void *ar[] = {bfAfter->v, NULL};
    Out("DebugStack: @^0\n", ar);

    i64 total = bf->v->total;
    i64 totalAfter = bfAfter->v->total;
    r |= Test(total > totalAfter, "Stack printout has less characters than before", NULL); 

    Return(m, r);
}
