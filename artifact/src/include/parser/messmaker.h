enum token_flags {
    TOKEN_CLOSE = 1 << 8;
};

typedef struct token {
    Type type;
    cls typeOf;
    i16 captureKey;
} Token;
