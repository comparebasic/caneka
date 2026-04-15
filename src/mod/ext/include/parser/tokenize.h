enum token_flags {
    TOKEN_SEPERATE = 1 << 8,
    TOKEN_OUTDENT = 1 << 9,
    TOKEN_INLINE = 1 << 10,
    TOKEN_BY_TYPE = 1 << 11,
    TOKEN_ATTR_KEY = 1 << 12,
    TOKEN_ATTR_VALUE = 1 << 13,
    TOKEN_NO_COMBINE = 1 << 14,
    TOKEN_NO_CONTENT = 1 << 15,
};

typedef struct tokenize {
    Type type;
    i16 captureKey;
    cls typeOf;
} Tokenize;

Tokenize *Tokenize_Make(MemCh *m, word captureKey, word flags, cls typeOf);
