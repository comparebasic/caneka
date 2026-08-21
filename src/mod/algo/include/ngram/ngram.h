#define NVal_Coord(pos, idx) ((pos)+sizeof(util)+(sizeof(NVal)*idx))

enum ngram_states {
    NGRAM_VALUE = 1 << 8,
    NGRAM_JUMP = 1 << 9,
    NGRAM_AMMEND = 1 << 10,
    NGRAM_FOUND = 1 << 11,
};

typedef struct ngram_val {
    i32 value;
    i32 addr;
} NVal;

/* 
typedef struct ngram_rec {
    util meaning;
    NVal val;
    NVal nth;
     .. nth1 .. 
     .. nth2 ..
     .. etc ..
} NRec;
*/

typedef struct ngram {
    Type type;
    RangeType objRange;
    i32 value;
    util meaning;
    Buff *bf;
    Str *s;
    void *source;
} NGram;

NGram *NGram_Make(MemCh *m,  Buff *bf, i16 stride, void *source);
