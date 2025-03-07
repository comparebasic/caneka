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

Single *Time64_Wrapped(MemCtx *m, time64_t n){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_TIME64;
    sgl->val.value = (util)n;
    return sgl;
}

String *Time_Today(MemCtx *m){
    struct timespec ts;
    clock_gettime(0, &ts);
    return TimeSpec_ToDayString(m, &ts);
}

String *TimeSpec_ToDayString(MemCtx *m, struct timespec *ts){
    struct tm *t = localtime(&(ts->tv_sec));
    String *s = String_Init(m, STRING_EXTEND);
    size_t l = strftime((char *)s->bytes, (size_t)String_GetSegSize(s), "%Y-%m-%d", t);
    s->length = l; 
    return s;
}
