#define BINSEG_SEG_MAX 1024

enum binseg_kinds {
    BINSEG_TYPE_BYTES = 1,
    BINSEG_TYPE_NUMBER = 2,
    BINSEG_TYPE_COLLECTION = 3,
    BINSEG_TYPE_BYTES_COLLECTION = 4,
    BINSEG_TYPE_DICTIONARY = 5,
    BINSEG_TYPE_INST = 6,
    BINSEG_TYPE_INST_TYPE = 7,
};

/* footprints:
   
   - Binary: Header + sizeof(byte) * hdr->total
   - Number: Header + sizeof(i32)
   - Collection: Header + sizeof(id) * hdr->total
        order: item,item,item...
   - Dictionary: Header + sizeof(id) * hdr->total * 2
        order: key,value,key,value,key,value...
   - Inst: Header + sizeof(id) * (hdr->total + 2)
        order: type, id,id,...
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
    Span *shelves;
    Span *records;
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
status BinSegCtx_SendEntry(BinSegCtx *ctx, struct binseg_hdr *hdr, Str *entry);
status BinSegCtx_Load(BinSegCtx *ctx);

Str *BinSegCtx_KindName(i8 kind);

status BinSeg_Init(MemCh *m);
