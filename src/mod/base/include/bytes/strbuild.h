typedef struct str_build {
    Type type;
    Span *p;
    void *args;
} StrBuild;

StrBuild *StrBuild_From(MemCh *m, byte *b, i16 length);
