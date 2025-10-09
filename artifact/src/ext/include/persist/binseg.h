#define BINSEG_SEG_MAX 1024

enum binseg_kinds {
    BINSEG_TYPE_BINARY = 1,
    BINSEG_TYPE_COLLECTION = 2,
    BINSEG_TYPE_BINARY_SEG = 3,
    BINSEG_TYPE_DICTIONARY = 4,
    BINSEG_TYPE_NODE = 6,
};

/* footprints:
   
   - Binary: Header + sizeof(byte) * hdr->total
   - Collection: Header + sizeof(hdr) * hdr->total
        order: item,item,item...
   - Dictionary: Header + sizeof(hdr) * hdr->total * 2
        order: key,value,key,value,key,value...
   - Node: Header + sizeof(hdr) * (hdr->total + 2)
        order: name,atts,child,child,child...
*/

enum binseg_types {
    BINSEG_REVERSED = 1 << 8,
    BINSEG_VISIBLE = 1 << 9,
};

typedef i32 (*BinSegIdxFunc)(struct binseg_ctx *ctx, Abstract *arg);
typedef i64 (*BinSegFunc)(struct binseg_ctx *ctx, Abstract *a);

typedef struct binseg_hdr {
    quad total;
    byte kind;
    byte id;
} BinSegHeader;

typedef struct binseg_ctx{
    Type type;
    Stream *sm;
    Table *cortext;
    BinSegIdxFunc func;
    Abstract *root;
    Abstract *value;
    Abstract *source;
} BinSegCtx;


extern struct lookup *BinSegLookup;
status BinSeg_Init(MemCh *m);
i32 BinSeg_IdxCounter(BinSegCtx *ctx, Abstract *arg);
BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source);
i64 BinSeg_ToStream(BinSegCtx *ctx, Abstract *a);
status BinSeg_LoadStream(BinSegCtx *ctx);
Str *BinSeg_KindName(i8 kind);
