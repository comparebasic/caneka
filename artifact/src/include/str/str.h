typedef struct str {
    Type type;
    word length;
    word allocated;
    byte *bytes;
} Str;

/* copy bytes into a new string */
Str *Str_From(MemCtx *m, byte *bytes, word length);
/* make a string that points to bytes */
Str *Str_Ref(MemCtx *m, byte *bytes, word length, word alloc);
/* setup an existing string struct */
status Str_Init(Str *s, byte *bytes, word length, word alloc);
/* make a blank allocated string */
Str *Str_Make(MemCtx *m, word alloc);
char *Str_ToCstr(MemCtx *m, Str *s);
