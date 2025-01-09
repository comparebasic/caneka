typedef struct cursor {
    Type type;
    String *s;
    String *seg;
    StrSnipAbs position;
    Span *spaces;
} Cursor;

status _Cursor_Init(MemCtx *m, Cursor *cur, String *s);
status Cursor_Init(MemCtx *m, Cursor *cur, String *s);
Cursor *Cursor_Make(MemCtx *m, String *s);

status Cursor_AddMatch(Cursor *cur, int length);
status Cursor_AddGap(Cursor *cur, int length);
status Cursor_Incr(Cursor *cur, int length);

String *Cursor_ToString(MemCtx *m, Cursor *cur, String *s);
