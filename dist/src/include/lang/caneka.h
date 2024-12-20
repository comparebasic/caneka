enum cnk_lang_range {
    _CNK_LANG_START = _TYPE_APPS_END,
    CNK_LANG_START,
    TYPE_LANG_CNK,
    TYPE_LANG_CNK_MODULE,
    TYPE_LANG_CNK_MOD_REF,
    _TYPE_LANG_CNK_END,
    _CNK_LANG_MAJOR,
    CNK_LANG_STRUCT,
    CNK_LANG_REQUIRE,
    CNK_LANG_PACKAGE,
    CNK_LANG_TYPE,
    CNK_LANG_ARG_LIST,
    CNK_LANG_CURLY_OPEN,
    _CNK_LANG_MAJOR_END,
    CNK_LANG_BLANK_LINE,
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
    CNK_LANG_FUNC_PTR,
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
    _CNK_LANG_END,
};

status CnkLang_Init(MemCtx *m);
Roebling *CnkLangCtx_RblMake(MemCtx *m, FmtCtx *ctx, RblCaptureFunc Capture);
FmtCtx *CnkLangCtx_Make(MemCtx *m, Abstract *source); /* Formatter < MemHandle */
status CnkLangCtx_Start(FmtCtx *ctx); /* Formatter < MemHandle */
/* utils */
String *CnkLang_requireFromSpan(MemCtx *m, Span *p);

char * CnkLang_RangeToChars(word range);
/* format to functs */
Abstract *CnkLang_RequireTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_StartTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_StructTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
status CnkLang_AddDefs(FmtCtx *ctx);
Span *CnkLang_GetModules(MemCtx *m);
