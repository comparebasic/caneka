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
    BINSEG_RECORDS = 1 << 10,
    BINSEG_SEEL 1 << 11,
};

typedef status (*BinSegFunc)(struct binseg_ctx *ctx, void *a, i16 id);

typedef struct binseg_ctx {
    Type type;
    Buff *bf;
    Table *cortext;
    Span *records;
    struct {
        Span *insts;
        Iter ords;
    } schema;
} BinSegCtx;

extern struct lookup *BinSegLookup;

BinSegCtx *BinSegCtx_Make(Buff *bf, word flags, Span *seels);
status BinSegCtx_Send(BinSegCtx *ctx, void *a, i16 id);
status BinSegCtx_SendEntry(BinSegCtx *ctx, struct binseg_hdr *hdr, Str *entry);
status BinSegCtx_Load(BinSegCtx *ctx);

Str *BinSegCtx_KindName(i8 kind);

status BinSeg_Init(MemCh *m);
