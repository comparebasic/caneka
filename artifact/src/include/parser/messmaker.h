enum token_flags {
    TOKEN_CLOSE = 1 << 8,
    TOKEN_CLOSE_OUTDENT = 1 << 9,
    TOKEN_LAST_VALUE = 1 << 10,
    TOKEN_LAST_SEP = 1 << 11,
};

typedef struct token {
    Type type;
    i16 captureKey;
    cls *typeOf;
} Token;

Token *Token_Make(MemCh *m, word captureKey, word flags, cls *typeOf);
