typedef struct strcursor {
    String *s;
    status state;
    String *seg;
    i64 position;
    i64 localPosition;
    i64 segIdx;
    i64 immidiateLength;
} SCursor;

typedef struct strcursor_range {
    status state;
    i64 compare;
    String *search;
    SCursor start;
    SCursor end;
    int length;
} Range;

SCursor* SCursor_Make(MemCtx *m, String *s);
status SCursor_Prepare(SCursor *sc, i64 length);
status SCursor_Incr(SCursor *sc, i64 length);
status SCursor_Reset(SCursor *sc);
status SCursor_Find(Range *range, Match *search);
status SCursor_SetLocals(SCursor *sc);
status Range_Set(Range *range, String *s);
status Range_Reset(Range *range, int anchor);
