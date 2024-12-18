enum cnk_lang_range {
    CNK_LANG_START = 1,
    _CNK_LANG_MAJOR,
    CNK_LANG_STRUCT,
    CNK_LANG_REQUIRE,
    CNK_LANG_PACKAGE,
    CNK_LANG_TYPE,
    CNK_LANG_ARG_LIST,
    CNK_LANG_CURLY_OPEN,
    _CNK_LANG_MAJOR_END,
    CNK_LANG_LINE,
    CNK_LANG_INDENT,
    CNK_LANG_C,
    CNK_LANG_END_C,
    CNK_LANG_TOKEN,
    CNK_LANG_TOKEN_DOT,
    CNK_LANG_TOKEN_NULLABLE,
    CNK_LANG_POST_TOKEN,
    _CNK_LANG_OP_START,
    CNK_LANG_OP,
    CNK_LANG_ASSIGN,
    CNK_LANG_ADD_AND,
    CNK_LANG_GT,
    CNK_LANG_LT,
    CNK_LANG_IS,
    CNK_LANG_NOT,
    CNK_LANG_EQ,
    CNK_LANG_CMP,
    CNK_LANG_FLAG_ADD,
    CNK_LANG_FLAG_SUB,
    _CNK_LANG_OP_END,
    CNK_LANG_VALUE,
    CNK_LANG_INVOKE,
    _CNK_LANG_CLOSER,
    CNK_LANG_LIST_CLOSE,
    CNK_LANG_CURLY_CLOSE,
    _CNK_LANG_CLOSER_END,
    _CNK_LANG_RETURNS,
    CNK_LANG_LINE_END,
    _CNK_LANG_RETURNS_END,
    _CNK_LANG_OBJ,
    CNK_LANG_ROEBLING,
    CNK_LANG_KEYS,
    CNK_LANG_IDXS,
    CNK_LANG_SEQ,
    _CNK_LANG_OBJ_END,
};

Roebling *CnkLangCtx_RblMake(MemCtx *m, FmtCtx *ctx, RblCaptureFunc Capture);
FmtCtx *CnkLangCtx_Make(MemCtx *m, Abstract *source); /* Formatter < MemHandle */
/* utils */
String *CnkLang_requireFromSpan(MemCtx *m, Span *p);

char * CnkLang_RangeToChars(word range);
/* format to functs */
Abstract *CnkLang_RequireTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_Start(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_StructTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
