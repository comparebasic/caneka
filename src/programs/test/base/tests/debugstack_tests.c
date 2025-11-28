#include <external.h>
#include <caneka.h>
#include <test.h>
#include "tests.h"

status DebugStack_Tests(MemCh *m){
    status r = READY;

    Buff *bf = Buff_Make(m, ZERO);
    Buff *bfAfter = Buff_Make(m, ZERO);

    DebugStack_Push(NULL, 0);
    DebugStack_Print(bf, 0);

    DebugStack_Pop();
    DebugStack_Print(bfAfter, 0);

    i64 total = bf->v->total;
    i64 totalAfter = bfAfter->v->total;
    r |= Test(total > totalAfter, "Stack printout has less characters than before", NULL); 

    DebugStack_Pop();
    return r;
}
