#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Time_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    struct timespec now;
    Time_Now(&now);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
