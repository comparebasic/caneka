#include <external.h>
#include <caneka.h>

i16 BinSegHeader_Size(quad total, word kind){
    i16 sz = sizeof(BinSegHeader);
    if(hdr->kind == BINSEG_TYPE_BYTES){
        sz += total;
    }else if(hdr->kind == BINSEG_TYPE_NUMBER){
        sz += sizeof(i32);
    }else if(hdr->kind == BINSEG_TYPE_NODE){
        sz += sizeof(i16)*(total+2);
    }else if(hdr->kind == BINSEG_TYPE_DICTIONARY){
        sz += sizeof(i16)*(total*2);
    }else{
        /* Collection/BytesCollection */
        sz += sizeof(i16)*total;
    }
    return sz
}

BinSegHeader *BinSegHeader_Make(BinSegCtx *ctx, quad total, word kind, word id, Str **content, Str **entry){
    MemCh *m = ctx->sm->m;
    Str *e ;

    i16 sz = BinSegHeader_Size(total, kind);
    i16 contentSz = e->alloc-sizeof(BinSegHeader);
    e = Str_Make(m, sz);
    *entry = e;
    if(ctx->type.state & BINSEG_REVERSED){
        if(content != NULL){
            *content = Str_Ref(m, e->bytes, contentSz, contentSz, ZERO);
        }
        return (BinSegHeader *)(e->bytes + e->length - sizeof(BinSegHeader));
    }else{
        if(content != NULL){
            *content = Str_Ref(m, e->bytes+sizeof(BinSegHeader), contentSz, contentSz, ZERO);
        }
        return (BinSegHeader *)e->bytes;
    }
}
