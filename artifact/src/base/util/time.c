#include <external.h>
#include <caneka.h>

#define TIME_BUFF_LEN 64

Str *Time64_ToStr(MemCh *m, Abstract *a){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_TIME64);
    Str *s = Str_Make(m, TIME_BUFF_LEN);
    struct timespec ts;
    struct tm tm;
    Time64_ToSpec(&ts, (time64_t)sg->val.value);

    gmtime_r(&ts.tv_sec, &tm);
    strftime((char *)s->bytes, TIME_BUFF_LEN, "%Y-%m-%dT%H:%M:%S.", &tm);
    s->length = strlen((char *)s->bytes);
    Str_AddI64(s, ts.tv_nsec);
    Str_Add(s, (byte *)"+00", 3);
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

Single *Time64_Wrapped(MemCh *m, time64_t n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_TIME64;
    sgl->val.value = (util)n;
    return sgl;
}

Str *Time_Today(MemCh *m){
    struct timespec ts;
    clock_gettime(0, &ts);
    return TimeSpec_ToDayStr(m, &ts);
}

Str *TimeSpec_ToDayStr(MemCh *m, struct timespec *ts){
    struct tm *t = localtime(&(ts->tv_sec));
    Str *s = Str_Make(m, STR_DEFAULT);
    size_t l = strftime((char *)s->bytes, (size_t)STR_DEFAULT, "%Y-%m-%d", t);
    s->length = l; 
    return s;
}
