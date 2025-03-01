typedef struct patchardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct patcountdef {
    word flags;
    i32 count;
} PatCountDef;

String *PatChar_FromString(MemCtx *m, String *s);
String *PatChar_KoFromString(MemCtx *m, String *s);
