#include <external.h>
#include <caneka.h>

status Time_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];

    struct timespec tm = {
        60*60*4,
        2000 
    };

    microTime mt = MicroTime_FromSpec(&tm);
    Str *fourHours2Kmillis = MicroTime_ToStr(m, mt);

    Str *expected = Str_CstrRef(m, "1970-01-01T04:00:00.2+00");
    args[0] = (Abstract *)fourHours2Kmillis;
    args[1] = NULL;
    r |= Test(Equals((Abstract *)fourHours2Kmillis, (Abstract *)expected), 
        "Time equates to four hours and 2k microseconds after January 1st 1970, have $", 
        args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
