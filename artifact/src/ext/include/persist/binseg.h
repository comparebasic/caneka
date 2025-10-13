#define BINSEG_SEG_MAX 1024

enum binseg_kinds {
    BINSEG_TYPE_BYTES = 1,
    BINSEG_TYPE_BYTES_COLLECTION = 2,
    BINSEG_TYPE_COLLECTION = 3,
    BINSEG_TYPE_DICTIONARY = 4,
    BINSEG_TYPE_NODE = 5,
    BINSEG_TYPE_NUMBER = 6,
    BINSEG_TYPE_INDEX = 7,
};

/* footprints:
   
   - Binary: Header + sizeof(byte) * hdr->total
   - Number: Header + sizeof(i32)
   - Collection: Header + sizeof(id) * hdr->total
        order: item,item,item...
   - Dictionary: Header + sizeof(id) * hdr->total * 2
        order: key,value,key,value,key,value...
   - Node: Header + sizeof(id) * (hdr->total + 2)
        order: name,atts,child,child,child...
*/

enum binseg_types {
    BINSEG_REVERSED = 1 << 8,
    BINSEG_VISIBLE = 1 << 9,
};

typedef i16 (*BinSegIdxFunc)(struct binseg_ctx *ctx, Abstract *arg);
typedef i64 (*BinSegFunc)(struct binseg_ctx *ctx, Abstract *a, i16 id);

typedef struct binseg_ctx{
    Type type;
    word _;
    word latestId;
    Stream *sm;
    Table *cortext;
    BinSegIdxFunc func;
    Table *keys;
    Table *tbl;
    Abstract *source;
} BinSegCtx;

extern struct lookup *BinSegLookup;

status BinSeg_Init(MemCh *m);
i16 BinSegCtx_IdxCounter(BinSegCtx *ctx, Abstract *arg);
BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source, word flags);
i64 BinSegCtx_ToStream(BinSegCtx *ctx, struct binseg_hdr *hdr, Str *sh);
i64 BinSegCtx_Send(BinSegCtx *ctx, Abstract *a, i16 id);
status BinSegCtx_LoadStream(BinSegCtx *ctx);
Str *BinSegCtx_KindName(i8 kind);
