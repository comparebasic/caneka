#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Time_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    struct timespec now;
    Time_Now(&now);

    struct timespec sub = {60, 0};
    struct timespec sub1m = {now.tv_sec, now.tv_nsec};
    Time_Sub(&sub1m, &sub);
    ApproxTime at;
    ApproxTime_Set(&now, &sub1m, &at);

    args[0] = &at;
    args[1] = NULL;
    r |= Test(at.type.state == APPROXTIME_MIN && at.value == 1,
        "Time delta set to 1 min @", args);

    sub.tv_sec = 0;
    sub.tv_nsec = 50000000;
    struct timespec sub50ms = {now.tv_sec, now.tv_nsec};
    Time_Sub(&sub50ms, &sub);
    ApproxTime_Set(&now, &sub50ms, &at);
    args[0] = &at;
    args[1] = NULL;
    r |= Test(at.type.state == APPROXTIME_MILLISEC && at.value == 50,
        "Time delta set to 50 millis @", args);

    DebugStack_Pop();
    return r;
}
