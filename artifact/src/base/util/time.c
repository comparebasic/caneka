#include <external.h>
#include <caneka.h>

#define TIME_BUFF_LEN 42

Str *Time64_ToStr(MemCtx *m, Abstract *a){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    String *s = String_Init(m, TIME_BUFF_LEN);
    struct timespec ts;
    struct tm tm;
    Time64_ToSpec(&ts, (time64_t)sg->val.value);

    gmtime_r(&ts.tv_sec, &tm);
    strftime((char *)s->bytes, TIME_BUFF_LEN, "%Y-%m-%dT%H:%M:%S.", &tm);
    s->length = strlen((char *)s->bytes);
    String_AddI64(m, s, ts.tv_nsec);
    String_AddBytes(m, s, bytes("+00"), 3);
    return s;
}

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
