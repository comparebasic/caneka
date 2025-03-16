typedef struct strvec_cursor {
    Type type;
    i32 pos;
    StrVec *v;
    byte *ptr;
    byte *end;
    Iter it;
} Cursor;

extern Cursor _strVecCurs;
