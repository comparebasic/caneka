enum cursor_flags {
    CURSOR_STR_BOUNDRY = 1 << 8,
};

typedef struct cursor {
    Type type;
    i32 offset;
    StrVec *v;
    util slot;
    byte *ptr;
    byte *end;
    Iter it;
} Cursor;

extern Cursor _strVecCurs;

status Cursor_NextByte(Cursor *curs);
status Cursor_Decr(Cursor *curs, i32 length);
status Cursor_Add(Cursor *curs, Str *s);
status Cursor_Setup(Cursor *curs, StrVec *v);
Cursor *Cursor_Make(MemCh *m, StrVec *v);
Cursor *Cursor_Copy(MemCh *m, Cursor *_curs);
status Cursor_NextByte(Cursor *curs);
status Cursor_Incr(Cursor *curs, i32 length);
StrVec *Cursor_Get(MemCh *m, Cursor *curs, i32 length, i32 offset);
status Cursor_FillStr(Cursor *curs, Str *s, i32 max);
