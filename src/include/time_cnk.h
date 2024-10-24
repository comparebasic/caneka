time64_t Time64_FromSpec(struct timespec *ts);
void Time64_ToSpec(struct timespec *ts, time64_t tm);
time64_t Time64_FromMillis(i64 millis);
i64 Time64_ToMillis(time64_t tm);
