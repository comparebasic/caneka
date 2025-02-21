typedef struct patchardef {
    word flags;
    word from;
    word to;
} PatCharDef;

String *PatChar_FromString(MemCtx *m, String *s);
String *PatChar_KoFromString(MemCtx *m, String *s);
