typedef struct strcursor_range {
    Type type;
    status state;
    i64 compare;
    String *search;
    struct strcursor start;
    struct strcursor end;
    int head;
    int tail;
} Range;

status Range_Set(Range *range, String *s);
status Range_Reset(Range *range, int anchor);
String *Range_Copy(MemCtx *m, Range *range);
status Range_Incr(Range *range);
status Range_IncrLead(Range *range);
status Range_Next(Range *range);
int Range_GetLength(Range *range);
byte Range_GetByte(Range *range);
