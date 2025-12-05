#include <external.h>
#include "base_module.h"

#define TIME_BUFF_LEN 64

void Time_Sub(struct timespec *ts, struct timespec *sub){
    ts->tv_sec -= sub->tv_sec;
    ts->tv_nsec -= sub->tv_nsec;
    if(ts->tv_nsec < 0){
        ts->tv_sec--;
        ts->tv_nsec += 1000000000;
    }
}

void Time_Add(struct timespec *ts, struct timespec *add){
    ts->tv_sec += add->tv_sec;
    ts->tv_nsec += add->tv_nsec;
    if(ts->tv_nsec >= 1000000000){
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000;
    }
}

void Time_Now(struct timespec *ts){
    clock_gettime(0, ts);
}

void Time_Delay(struct timespec *ts, struct timespec *remaining){
    nanosleep(ts, remaining);
}

Str *Time_ToStr(MemCh *m, struct timespec *ts){
    Str *s = Str_Make(m, TIME_BUFF_LEN);
    struct tm value;
    gmtime_r(&ts->tv_sec, &value);
    strftime((char *)s->bytes, TIME_BUFF_LEN, "%Y-%m-%dT%H:%M:%S.", &value);
    s->length = strlen((char *)s->bytes);
    Str_AddI64(s, ts->tv_nsec);
    Str_Add(s, (byte *)"+00", 3);
    return s;
}

boolean Time_Greater(struct timespec *ts, struct timespec *add){
    if(ts->tv_sec > add->tv_sec || ts->tv_sec == add->tv_sec && 
            ts->tv_nsec > add->tv_nsec){
        return TRUE;
    }
    return FALSE;
}

boolean Time_Beyond(struct timespec *ts, struct timespec *add, struct timespec*amount){
    struct timespec _ts;
    _ts.tv_sec = ts->tv_sec;
    _ts.tv_nsec = ts->tv_nsec;
    Time_Sub(&_ts, add);
    if(_ts.tv_sec > amount->tv_sec || _ts.tv_sec == amount->tv_sec && 
            _ts.tv_nsec > amount->tv_nsec){
        return TRUE;
    }
    return FALSE;
}

Str *Time_Today(MemCh *m){
    struct timespec ts;
    clock_gettime(0, &ts);
    return Time_ToDayStr(m, &ts);
}

Str *Time_ToDayStr(MemCh *m, struct timespec *ts){
    struct tm *t = localtime(&(ts->tv_sec));
    Str *s = Str_Make(m, STR_DEFAULT);
    size_t l = strftime((char *)s->bytes, (size_t)STR_DEFAULT, "%Y-%m-%d", t);
    s->length = l; 
    return s;
}

Single *Time_Wrapped(MemCh *m, struct timespec *ts){
    Single *sg = Ptr_Wrapped(m, ts, TYPE_TIMESPEC); 
    sg->type.of = TYPE_WRAPPED_PTR_TIMESPEC;
    return sg;
}

status ApproxTime_Beyond(struct timespec *delta, ApproxTime *mt){
    if((mt->type.state & APPROXTIME_MILLISEC) &&
            delta->tv_nsec > mt->value * 1000000){
        return SUCCESS;
    }else if((mt->type.state & APPROXTIME_SEC) &&
            delta->tv_sec > mt->value){
        return SUCCESS;
    }else if((mt->type.state & APPROXTIME_HOUR) &&
            delta->tv_sec > mt->value*60*60){
        return SUCCESS;
    }else if((mt->type.state & APPROXTIME_DAY) &&
            delta->tv_sec > mt->value*60*60*24){
        return SUCCESS;
    }
    return NOOP;
}

status ApproxTime_Set(struct timespec *present, struct timespec *start, ApproxTime *at){
    memset(at, 0, sizeof(ApproxTime));
    at->type.of = TYPE_APPROXTIME;

    struct timespec ts;
    ts.tv_sec = present->tv_sec - start->tv_sec;
    ts.tv_nsec = present->tv_nsec - start->tv_nsec;

    if(ts.tv_sec >= 60*60*24){
        at->type.state = APPROXTIME_DAY;
        at->value = ts.tv_sec / (60*60*24);
        return SUCCESS;
    }else if(ts.tv_sec >= 60*60){
        at->type.state = APPROXTIME_HOUR;
        at->value = ts.tv_sec / (60*60);
        return SUCCESS;
    }else if(ts.tv_sec >= 60){
        at->type.state = APPROXTIME_MIN;
        at->value = ts.tv_sec / 60;
        return SUCCESS;
    }else if( ts.tv_sec > 0){
        at->type.state = APPROXTIME_SEC;
        at->value = (i32) ts.tv_sec;
    }else if(ts.tv_nsec > at->value * 1000000){
        at->type.state = APPROXTIME_MILLISEC;
        at->value = (i32) (ts.tv_nsec / 1000000);
        return SUCCESS;
    }

    return NOOP;
}
