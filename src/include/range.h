typedef struct strcursor_range {
    Type type;
    status state;
    i64 compare;
    String *search;
    struct strcursor start;
    struct strcursor potential;
    struct strcursor end;
} Range;

status Range_Set(Range *range, String *s);
String *Range_Copy(MemCtx *m, Range *range);
status Range_Sync(Range *range, SCursor *cursor);
int Range_GetLength(Range *range);
byte Range_GetByte(Range *range);
