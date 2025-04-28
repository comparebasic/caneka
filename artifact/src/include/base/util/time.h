Str *Time64_ToStr(MemCh *m, time64_t t);
time64_t Time64_FromSpec(struct timespec *ts);
time64_t Time64_Now();
void Time64_ToSpec(struct timespec *ts, time64_t tm);
time64_t Time64_FromMillis(i64 millis);
i64 Time64_ToMillis(time64_t tm);
Single *Time64_Wrapped(MemCh *m, time64_t n);
Str *Time_Today(MemCh *m);
Str *TimeSpec_ToDayStr(MemCh *m, struct timespec *ts);
