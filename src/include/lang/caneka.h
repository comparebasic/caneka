enum cnk_lang_range {
    CNK_LANG_START = 1;
    CNK_LANG_LINE;
    CNK_LANG_TYPE;
    CNK_LANG_C;
    CNK_LANG_VARDEF;
    CNK_LANG_ASSIGN_VAL;
};

typedef struct caneka_lang_space {
    Type type;
    word spaceType;
    Abstract *space;
    struct caneka_lang_space *parent;
} CnkLangSpace;

typedef struct caneka_lang_ctx {
    Type type;
    CnkLangSpace *space;
} CnkLangCtx;
