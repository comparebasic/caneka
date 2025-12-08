enum pat_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* X */
    PAT_OPTIONAL = 1 << 1, /* P */
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_INVERT = 1 << 4, /* I */
    PAT_COUNT = 1 << 5, /* C */
    PAT_INVERT_CAPTURE = 1 << 6, /* G */
    PAT_KO = 1 << 7, /* K */
    PAT_KO_TERM = 1 << 8, /* O */
    PAT_SINGLE = 1 << 9, /* S */
    PAT_LEAVE  = 1 << 10, /* L */
    PAT_DROPOUT = 1 << 11, /* D */
    PAT_GO_ON_FAIL = 1 << 12, /* T */
    PAT_CONSUME = 1 << 13, /* U */
    PAT_REQUIRE_KO = 1 << 14 /* R */
};

typedef struct patchardef {
    word flags;
    byte from;
    byte to;
} PatCharDef;

typedef struct patcountdef {
    word flags;
    i16 count;
} PatCountDef;

PatCharDef *PatChar_FromStr(MemCh *m, Str *s);
PatCharDef *PatChar_KoFromStr(MemCh *m, Str *s);
PatCharDef *PatChar_FromStrVec(MemCh *m, StrVec *v);
