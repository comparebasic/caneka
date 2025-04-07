enum str_flags {
    STRING_CONST = 1 << 8,
};

typedef struct str {
    Type type;
    word length;
    word alloc;
    byte *bytes;
} Str;

extern Str *NL;

Str *Str_From(MemCh *m, byte *bytes, word length);
Str *Str_Ref(MemCh *m, byte *bytes, word length, word alloc);
status Str_Init(Str *s, byte *bytes, word length, word alloc);
Str *Str_Make(MemCh *m, word alloc);
char *Str_ToCstr(MemCh *m, Str *s);
i64 Str_ToFd(Str *s, int fd);
i64 Str_Add(Str *s, byte *b, i64 length);
i64 Str_AddCstr(Str *s, char *cstr);
i64 Out(MemCh *m, char *fmt, ...);
char *Str_Cstr(MemCh *m, Str *s);
Str *Str_CstrRef(MemCh *m, char *cstr);
i64 Str_AddCstr(Str *s, char *cstr);
Str *Str_Clone(MemCh *m, Str *s, word alloc);
status Str_Wipe(Str *s);
status Str_Reset(Str *s);
