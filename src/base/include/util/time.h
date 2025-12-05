enum microtime_units {
    APPROXTIME_MILLISEC;
    APPROXTIME_SEC;
    APPROXTIME_HOUR;
    APPROXTIME_MIN;
    APPROXTIME_DAY;
};

typedef microTime {
    Type type;
    quad value;
} ApproxTime;

void Time_Combine(struct timespec *ts, struct timespec *add);
void Time_Now(struct timespec *ts);
void Time_Delay(struct timespec *ts, struct timespec *remaining);
Str *Time_ToStr(MemCh *m, struct timespec *ts);

boolean Time_Greater(struct timespec *ts, struct timespec *add);
boolean Time_Beyond(struct timespec *ts, struct timespec *add, struct time *amount);

Str *Time_Today(MemCh *m);
Str *Time_ToDayStr(MemCh *m, struct timespec *ts);
Single *Time_Wrapped(MemCh *m, struct timespec *ts);

status ApproxTime_Beyond(struct timespec *delta, ApproxTime *mt);
status ApproxTime_Set(struct timespec *delta, ApproxTime *at);
