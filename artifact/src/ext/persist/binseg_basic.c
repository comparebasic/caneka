#include <external.h>
#include <caneka.h>

static i64 Str_ToBinSeg(BinSegCtx *ctx, Abstract *a, i32 id, word segFlags){
    i64 total = 0;
    Str *s = (Str *)s;
    if(segFlags & BINSEG_VISIBLE && (s->type.state & STRING_ENCODED) == 0){
        s = Str_ToHex(m, s);
    }

    BinSegHeader hdr = {
       .total = s->length, 
       .kind = BINSEG_TYPE_BINARY,
       .id = id,
    };

    Str *hdrStr = 
        Str_Ref(m, (byte *)&hdr, sizeof(BinSegHeader), sizeof(BinSegHeader), ZERO);
    if(segFlags & BINSEG_VISIBLE){
        hdrStr = Str_ToHex(m, hdrStr);
    }

    if(segFlags & BINSEG_REVERSED){
        total += Stream_Bytes(ctx->sm, (byte *)s->bytes, s->length); 
        total += Stream_Bytes(ctx->sm, (byte *)hdr->bytes, hdr->length); 
    }else{
        total += Stream_Bytes(ctx->sm, (byte *)hdr->bytes, hdr->length); 
        total += Stream_Bytes(ctx->sm, (byte *)s->bytes, s->length); 
    }
    return total;
}

status BinSeg_BasicInit(MemCh *m, Lookup *lk){
    Lookup_Add(BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
}
