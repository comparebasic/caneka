typedef struct patchardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct patcountdef {
    word flags;
    i32 count;
} PatCountDef;

Str *PatChar_FromStr(MemCh *m, Str *s);
Str *PatChar_KoFromStr(MemCh *m, Str *s);
