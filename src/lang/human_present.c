#include <external.h>
#include <caneka.h>

#define TIME_BUFF_LEN 42

String *Time64_Present(MemCtx *m, Abstract *a){
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

String *I64_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_I64);
    return NULL;
}

String *Util_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_UTIL);
    return NULL;
}

String *Bool_Present(MemCtx *m, Abstract *s){
    Single *sg = (Single *)as(s, TYPE_WRAPPED_BOOL);
    return NULL;
}
