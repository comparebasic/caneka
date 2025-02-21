#define NGRAM_SEG_LENGTH 14

enum ngram_flags8 {
    NGRAM_SPACE = 1 << 0,
    NGRAM_START = 1 << 1,
    NGRAM_CONTENT = 1 << 2,
    NGRAM_BRANCHES = 1 << 3,
    NGRAM_SEALED = 1 << 4,
    NGRAM_TRUNC = 1 << 5,
    NGRAM_JUMP = 1 << 6,
    NGRAM_NEW = 1 << 7,
};

typedef struct ngram_content {
    flags8 flags;
    i8 length;
    byte content[NGRAM_SEG_LENGTH];
} NgramContent;

typedef struct ngram_content {
    flags8 flags;
    byte _0;
    byte _1;
    byte _2;
    int _3;
    void *next;
} NgramJump;

typedef i64[2] NgramSpace;

typedef struct ngram_branches {
    byte low;
    byte high;
    byte length;
    byte _1;
    i32 count;
    NgramContent *ct;
} Ngram;
