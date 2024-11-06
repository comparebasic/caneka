#include <external.h>
#include <caneka.h>

time64_t Time64_FromSpec(struct timespec *ts){
    return (ts->tv_sec * 1000000000) + ts->tv_nsec;  
}

time64_t Time64_Now(){
    struct timespec ts;
    clock_gettime(0, &ts);
    return Time64_FromSpec(&ts);
}

void Time64_ToSpec(struct timespec *ts, time64_t tm){
    ts->tv_sec = tm / 1000000000;
    ts->tv_nsec = tm % 1000000000; 
}

time64_t Time64_FromMillis(i64 millis){
    return millis * 1000000;
}

i64 Time64_ToMillis(time64_t tm){
    return tm / 1000000;
}
