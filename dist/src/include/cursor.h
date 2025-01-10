typedef struct cursor {
    Type type;
    byte *ptr;
    String *s;
    String *seg;
    StrSnipAbs position;
    String *snips;
} Cursor;

status _Cursor_Init(MemCtx *m, Cursor *cur, String *s);
status Cursor_Init(MemCtx *m, Cursor *cur, String *s);
Cursor *Cursor_Make(MemCtx *m, String *s);

status Cursor_AddMatch(Cursor *cur, int length);
status Cursor_AddGap(Cursor *cur, int length);
status Cursor_Incr(Cursor *cur, int length);

String *Cursor_ToString(MemCtx *m, Cursor *cur, String *s);
#define Cursor_GetByte(cur) (*((cur)->ptr))
