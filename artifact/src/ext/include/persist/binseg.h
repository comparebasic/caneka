#define BINSEG_SEG_MAX 1024

enum binseg_kinds = {
    BINSEG_TYPE_BINARY = 1,
    BINSEG_TYPE_COLLECTION = 2,
    BINSEG_TYPE_BINARY_SEG = 3,
    BINSEG_TYPE_DICTIONARY = 4,
    BINSEG_TYPE_NODE = 6,
}

enum binseg_types {
    BINSEG_REVERSED = 1 << 8,
    BINSEG_VISIBLE = 1 << 9,
};

i32 (*BinSegIdxFunc)(struct binseg_ctx *ctx, Abstract *arg);
i64 (*BinSegFunc)(struct binseg_ctx *ctx, Abstract *a, word segFlags);

typedef binseg_hdr {
    quad total;
    byte kind;
    byte id;
} BinSegHeader;

typedef struct binseg_ctx{
    Type type;
    Stream *sm;
    Table *cortext;
    BinSegIdxFunc func;
    Source *source;
} BinSegCtx;


extern struct lookup *BinSegLookup;
status BinSeg_Init(MemCh *m, Lookup *lk);
i32 BinSeg_IdxCounter(BinSegCtx *ctx);
BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source);
