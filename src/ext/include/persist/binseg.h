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

typedef i16 (*BinSegIdxFunc)(struct binseg_ctx *ctx, void *arg);
typedef status (*BinSegFunc)(struct binseg_ctx *ctx, void *a, i16 id);

typedef struct binseg_ctx{
    Type type;
    word _;
    word latestId;
    Buff *bf;
    Table *cortext;
    BinSegIdxFunc func;
    Table *keys;
    Table *tbl;
    void *source;
} BinSegCtx;

extern struct lookup *BinSegLookup;

status BinSeg_Init(MemCh *m);
i16 BinSegCtx_IdxCounter(BinSegCtx *ctx, void *arg);
BinSegCtx *BinSegCtx_Make(Buff *bf, BinSegIdxFunc func, void *source, word flags);
status BinSegCtx_ToBuff(BinSegCtx *ctx, struct binseg_hdr *hdr, Str *sh);
status BinSegCtx_Send(BinSegCtx *ctx, void *a, i16 id);
status BinSegCtx_Load(BinSegCtx *ctx);
Str *BinSegCtx_KindName(i8 kind);
