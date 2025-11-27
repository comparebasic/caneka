#define BINSEG_SEG_MAX 1024

enum binseg_kinds {
    BINSEG_TYPE_BYTES = 1,
    BINSEG_TYPE_NUMBER = 2,
    BINSEG_TYPE_COLLECTION = 3,
    BINSEG_TYPE_BYTES_COLLECTION = 4,
    BINSEG_TYPE_DICTIONARY = 5,
    BINSEG_TYPE_INST = 6,
};

enum binseg_types {
    BINSEG_REVERSED = 1 << 8,
    BINSEG_VISIBLE = 1 << 9,
    BINSEG_RECORDS = 1 << 10,
    BINSEG_SEEL 1 << 11,
};

typedef status (*BinSegFunc)(struct binseg_ctx *ctx, void *a, i16 id);

/* 
 * serialized as {Header}[{Entry}]
 *
 * Types with Entries:
 * - Inst: instOf(i16)
 * - Str: length(bytes)
 * - Number: value(i64) 
 */

typedef struct binseg_ctx {
    Type type;
    Buff *bf;
    Span *shelves;
    Span *records;
    word nextId;
    struct {
        Span *insts;
        Iter ords;
    } schema;
    Iter it;
} BinSegCtx;

typedef binseg_ident {
    word idx;
    word id;
} BinSegIdent;

typedef struct binseg_hdr {
    word total;
    word kind;
    BinSegIdent ident;
} BinSegHeader;

extern struct lookup *BinSegLookup;

BinSegCtx *BinSegCtx_Make(Buff *bf, word flags, Span *seels);
status BinSegCtx_Send(BinSegCtx *ctx, void *a, i16 id);
status BinSegCtx_SendEntry(BinSegCtx *ctx, Str *entry);
status BinSegCtx_Load(BinSegCtx *ctx);
word BinSegCtx_HeaderSize(word kind, word length);

status BinSeg_Init(MemCh *m);
