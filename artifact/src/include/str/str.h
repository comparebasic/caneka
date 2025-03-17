typedef struct str {
    Type type;
    word length;
    word alloc;
    byte *bytes;
} Str;

Str *Str_From(MemCtx *m, byte *bytes, word length);
Str *Str_Ref(MemCtx *m, byte *bytes, word length, word alloc);
status Str_Init(Str *s, byte *bytes, word length, word alloc);
Str *Str_Make(MemCtx *m, word alloc);
char *Str_ToCstr(MemCtx *m, Str *s);
i64 Str_ToFd(Str *s, int fd);
i64 Str_Add(Str *s, byte *b, i64 length);
Str *Str_FromAbs(MemCtx *m, Abstract *a);
Str *Str_FromTyped(MemCtx *m, void *v, cls type);
i64 Out(MemCtx *m, char *fmt, ...);
char *Str_Cstr(MemCtx *m, Str *s);
