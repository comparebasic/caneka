enum cnk_lang_range {
    CNK_LANG_START = 1,
    CNK_LANG_LINE,
    CNK_LANG_INDENT,
    CNK_LANG_STRUCT,
    CNK_LANG_REQUIRE,
    CNK_LANG_PACKAGE,
    CNK_LANG_TYPE,
    CNK_LANG_C,
    CNK_LANG_END_C,
    CNK_LANG_TOKEN,
    CNK_LANG_TOKEN_DOT,
    CNK_LANG_POST_TOKEN,
    CNK_LANG_ASSIGN,
    CNK_LANG_VALUE,
};

typedef struct caneka_lang_space {
    Type type;
    word spaceType;
    Abstract *space;
    struct caneka_lang_space *parent;
} CnkLangSpace;

typedef struct caneka_lang_ctx {
    /* Transp */
    Type type;   
    MemCtx *m;
    String *src;
    String *dist;
    struct {
        String *source;
        String *dest;
        File sourceFile;
        File destFile;
    } current;
    OutFunc out;
    Roebling *rbl;
    /* End Transp */
    CnkLangSpace *space;
} CnkLangCtx;

CnkLangCtx *CnkLangCtx_Make(MemCtx *m);
CnkLangSpace *CnkLangSpace_Make(MemCtx *m);
