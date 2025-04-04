typedef struct strvec_cursor {
    Type type;
    i32 pos;
    StrVec *v;
    util slot;
    byte *ptr;
    byte *end;
    Iter it;
} Cursor;

extern Cursor _strVecCurs;
status Cursor_Setup(Cursor *curs, StrVec *v);
status Cursor_SetPos(Cursor *curs, i32 pos);
