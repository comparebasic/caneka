#include <external.h>
#include <caneka.h>

#define TIME_BUFF_LEN 64

microTime MicroTime_FromSpec(struct timespec *ts){
    return (ts->tv_sec * 1000l) + (ts->tv_nsec / 1000l);
}

void MicroTime_ToSpec(struct timespec *ts, microTime tm){
    ts->tv_sec = tm / 1000;
    ts->tv_nsec = tm % 1000; 
}

status Time_Delay(microTime tm, microTime *remaining){
    struct timespec ts;
    MicroTime_ToSpec(&ts, tm);
    struct timespec remain;
    nanosleep(&ts, &remain);
    if(remaining != NULL){
        *remaining = MicroTime_FromSpec(&remain);
    }
    return SUCCESS;
}

Str *MicroTime_ToStr(MemCh *m, microTime tm){
    Str *s = Str_Make(m, TIME_BUFF_LEN);
    struct timespec ts;
    struct tm value;
    MicroTime_ToSpec(&ts, tm);

    gmtime_r(&ts.tv_sec, &value);
    strftime((char *)s->bytes, TIME_BUFF_LEN, "%Y-%m-%dT%H:%M:%S.", &value);
    s->length = strlen((char *)s->bytes);
    Str_AddI64(s, ts.tv_nsec);
    Str_Add(s, (byte *)"+00", 3);
    return s;
}

microTime MicroTime_Now(){
    struct timespec ts;
    clock_gettime(0, &ts);
    return MicroTime_FromSpec(&ts);
}

microTime MicroTime_FromMillis(i64 millis){
    return millis * 1000000;
}

i64 MicroTime_ToMillis(microTime tm){
    return tm / 1000000;
}

Single *MicroTime_Wrapped(MemCh *m, microTime tm){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_TIME64;
    sgl->val.value = (util)tm;
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
