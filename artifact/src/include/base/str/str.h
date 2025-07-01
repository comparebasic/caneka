enum str_flags {
    STRING_TEXT = 1 << 8,
    STRING_CONST = 1 << 9,
    STRING_BINARY = 1 << 10,
    STRING_ENCODED = 1 << 11,
    STRING_FMT_ANSI = 1 << 12,
    STRING_COPY = 1 << 13,
    STRING_UTF8 = 1 << 14,
};

typedef struct str {
    Type type;
    word length;
    word alloc;
    byte *bytes;
} Str;

typedef status (*StrRslvFunc)(MemCh *m, i32 idx, Str *arg, Abstract *source);

boolean TextCharFilter(byte *b, i64 length);
status Str_Reset(Str *s);
status Str_Wipe(Str *s);
status Str_Incr(Str *s, word length);
i64 Str_ToFd(Str *s, int fd);
i64 Str_Add(Str *s, byte *b, i64 length);
i64 Str_AddCstr(Str *s, char *cstr);
char *Str_Cstr(MemCh *m, Str *s);
Str *Str_Clone(MemCh *m, Str *s, word alloc);
Str *Str_From(MemCh *m, byte *bytes, word length);
Str *Str_Ref(MemCh *m, byte *bytes, word length, word alloc, word flags);
Str *Str_CstrRef(MemCh *m, char *cstr);
status Str_Init(Str *s, byte *bytes, word length, word alloc);
Str *Str_Make(MemCh *m, word alloc);
