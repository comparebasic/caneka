typedef struct cursor {
    Type type;
    byte *ptr;
    String *s;
    String *seg;
    i64 offset;
    i64 local;
} Cursor;

status _Cursor_Init(Cursor *cur, String *s);
status Cursor_Init(Cursor *cur, String *s);
Cursor *Cursor_Make(MemCtx *m, String *s);

status Cursor_AddMatch(Cursor *cur, int length);
status Cursor_AddGap(Cursor *cur, int length);
status Cursor_Incr(Cursor *cur, int length);
status Cursor_Decr(Cursor *cur, int length);
status Cursor_Flush(MemCtx *m, Cursor *cur, OutFunc func, Abstract *source);

#define Cursor_GetByte(cur) (*((cur)->ptr))
#define Cursor_Total(cur) ((cur)->offset+(cur)->local)
