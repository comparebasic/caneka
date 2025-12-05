enum microtime_units {
    APPROXTIME_MILLISEC = 1 << 8,
    APPROXTIME_SEC = 1 << 9,
    APPROXTIME_MIN = 1 << 10,
    APPROXTIME_HOUR = 1 << 11,
    APPROXTIME_DAY = 1 << 12,
};

typedef struct approx_time {
    Type type;
    quad value;
} ApproxTime;

void Time_Sub(struct timespec *ts, struct timespec *sub);
void Time_Add(struct timespec *ts, struct timespec *add);
void Time_Now(struct timespec *ts);
void Time_Delay(struct timespec *ts, struct timespec *remaining);
Str *Time_ToStr(MemCh *m, struct timespec *ts);

boolean Time_Greater(struct timespec *ts, struct timespec *add);
boolean Time_Beyond(struct timespec *ts, struct timespec *add, struct timespec *amount);

Str *Time_Today(MemCh *m);
Str *Time_ToDayStr(MemCh *m, struct timespec *ts);
Single *Time_Wrapped(MemCh *m, struct timespec *ts);

status ApproxTime_Beyond(struct timespec *delta, ApproxTime *mt);
status ApproxTime_Set(struct timespec *delta, ApproxTime *at);
