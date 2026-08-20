enum ngram_states {
    NGRAM_VALUE = 1 << 8,
    NGRAM_JUMP = 1 << 9,
    NGRAM_AMMEND = 1 << 10,
    NGRAM_FOUND = 1 << 11,
};

typedef struct ngram_rec {
    Type type;
    i32 value;
} NRec;

typedef struct ngram {
    Type type;
    RangeType objRange;
    i32 value;
    Buff *bf;
    Str *s;
    void *source;
} NGram;

NGram *NGram_Make(MemCh *m,  Buff *bf, i16 stride, void *source);
