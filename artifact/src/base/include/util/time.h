typedef i64 microTime;

#define TIME_SEC 1000000l
#define TIME_MIN TIME_SEC*60
#define TIME_HOUR TIME_MIN*60
#define TIME_DAY TIME_HOUR*24

Str *MicroTime_ToStr(MemCh *m, microTime t);
microTime MicroTime_Now();
void MicroTime_ToSpec(struct timespec *ts, microTime tm);
microTime MicroTime_FromSpec(struct timespec *ts);
microTime MicroTime_FromMillis(i64 millis);
i64 MicroTime_ToMillis(microTime tm);
Single *MicroTime_Wrapped(MemCh *m, microTime n);
Str *Time_Today(MemCh *m);
Str *TimeSpec_ToDayStr(MemCh *m, struct timespec *ts);
microTime Time_Combine(microTime start, microTime add);
status Time_Delay(microTime tm, microTime *remaining);
