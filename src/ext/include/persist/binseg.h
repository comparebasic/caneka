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
    BINSEG_READ = 1 << 9,
    BINSEG_ADD = 1 << 10,
    BINSEG_MODIFY = 1 << 11,
};

typedef status (*BinSegFunc)(struct binseg_ctx *ctx, void *a, i16 id, i16 idx);

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
    MemCh *m;
    Buff *read;
    Buff *add;
    Buff *modify;
    Span *shelves;
    Span *records;
} BinSegCtx;

typedef struct binseg_ident {
    i16 idx;
    i16 id;
} BinSegIdent;

typedef struct binseg_hdr {
    word total;
    word kind;
    BinSegIdent ident;
} BinSegHeader;

extern struct lookup *BinSegLookup;
extern Table *BinSegActionNames;

status BinSegCtx_Open(BinSegCtx *ctx, Str *path);
BinSegCtx *BinSegCtx_Make(MemCh *m, word flags);
status BinSegCtx_SendByIdent(BinSegCtx *ctx, void *_a, i16 id, i16 idx);
status BinSegCtx_Load(BinSegCtx *ctx);
word BinSegCtx_HeaderSize(word kind, word length);
word BinSeg_ActionByStr(void *a);

#define BinSegCtx_Send(ctx, a) BinSegCtx_SendByIdent((ctx), (a), 0, 0)

status BinSeg_Init(MemCh *m);
