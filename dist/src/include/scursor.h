typedef struct strcursor {
    Type type;
    String *s;
    String *seg;
    i64 position;
    i64 total;
} SCursor;

SCursor* SCursor_Make(MemCtx *m, String *s);
status SCursor_Prepare(SCursor *sc, i64 length);
status SCursor_Incr(SCursor *sc, i64 length);
status SCursor_Reset(SCursor *sc);
status SCursor_Find(struct strcursor_range *range, Match *search);
status SCursor_SetLocals(SCursor *sc);
status SCursor_Init(SCursor *sc, String *s);
