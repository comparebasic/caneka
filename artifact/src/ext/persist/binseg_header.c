#include <external.h>
#include <caneka.h>

i16 BinSegHeader_FooterSize(quad total, word kind){
    i16 sz = 0;
    if(kind == BINSEG_TYPE_BYTES){
        sz += total;
    }else if(kind == BINSEG_TYPE_NUMBER){
        sz += sizeof(i32);
    }else if(kind == BINSEG_TYPE_NODE){
        sz += sizeof(i16)*(total+2);
    }else if(kind == BINSEG_TYPE_DICTIONARY){
        sz += sizeof(i16)*(total*2);
    }else{
        /* Collection/BytesCollection */
        sz += sizeof(i16)*total;
    }
    return sz;
}

BinSegHeader *BinSegHeader_Make(BinSegCtx *ctx, quad total, word kind, word id, Str **content, Str **entry){
    MemCh *m = ctx->bf->m;
    BinSegHeader *hdr;
    Str *e;

    i16 sz = BinSegHeader_FooterSize(total, kind)+sizeof(BinSegHeader);
    e = Str_Make(m, sz);
    e->length = e->alloc;
    i16 contentSz = e->alloc-sizeof(BinSegHeader);
    *entry = e;
    if(ctx->type.state & BINSEG_REVERSED){
        if(content != NULL){
            *content = Str_Ref(m, e->bytes, contentSz, contentSz, ZERO);
        }
        hdr = (BinSegHeader *)(e->bytes + e->alloc - sizeof(BinSegHeader));
    }else{
        if(content != NULL){
            *content = Str_Ref(m, e->bytes+sizeof(BinSegHeader), contentSz, contentSz, ZERO);
        }
        hdr =  (BinSegHeader *)e->bytes;
    }

    hdr->total = total;
    hdr->kind = kind;
    hdr->id = id;

    return hdr;
}
