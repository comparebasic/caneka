#include <external.h>
#include <caneka.h>

static i64 Span_ToBinSeg(BinSegCtx *ctx, Abstract *a, i32 id){
    i64 total = 0;
    MemCh *m = ctx->sm->m;
    Span *p = (Span *)as(a, TYPE_SPAN);
    Span *ids = Span_Make(m);
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        i32 id = ctx->func(ctx, NULL);
        Span_Add(ids, (Abstract *)I32_Wrapped(m, id));
        total += BinSegCtx_ToStream(ctx, Iter_Get(&it), id);
    };

    BinSegHeader hdr = {
       .total = p->nvalues, 
       .kind = BINSEG_TYPE_COLLECTION,
       .id = id,
    };

    Str *hdrStr = 
        Str_Ref(m, (byte *)&hdr, sizeof(BinSegHeader), sizeof(BinSegHeader), ZERO);
    if(ctx->type.state & BINSEG_VISIBLE){
        hdrStr = Str_ToHex(m, hdrStr);
    }

    if(ctx->type.state & BINSEG_REVERSED){
        total += BinSegCtx_FooterToStream(ctx, ids);
        total += Stream_Bytes(ctx->sm, (byte *)hdrStr->bytes, hdrStr->length); 
    }else{
        total += Stream_Bytes(ctx->sm, (byte *)hdrStr->bytes, hdrStr->length); 
        total += BinSegCtx_FooterToStream(ctx, ids);
    }

    return total;
}

static i64 Str_ToBinSeg(BinSegCtx *ctx, Abstract *a, i32 id){
    i64 total = 0;
    Str *s = (Str *)as(a, TYPE_STR);
    MemCh *m = ctx->sm->m;
    if(ctx->type.state & BINSEG_VISIBLE && (s->type.state & STRING_ENCODED) == 0){
        s = Str_ToHex(m, s);
    }

    BinSegHeader hdr = {
       .total = s->length, 
       .kind = BINSEG_TYPE_BYTES,
       .id = id,
    };

    Str *hdrStr = 
        Str_Ref(m, (byte *)&hdr, sizeof(BinSegHeader), sizeof(BinSegHeader), ZERO);
    if(ctx->type.state & BINSEG_VISIBLE){
        hdrStr = Str_ToHex(m, hdrStr);
    }

    if(ctx->type.state & BINSEG_REVERSED){
        total += Stream_Bytes(ctx->sm, (byte *)s->bytes, s->length); 
        total += Stream_Bytes(ctx->sm, (byte *)hdrStr->bytes, hdrStr->length); 
    }else{
        total += Stream_Bytes(ctx->sm, (byte *)hdrStr->bytes, hdrStr->length); 
        total += Stream_Bytes(ctx->sm, (byte *)s->bytes, s->length); 
    }
    return total;
}

status BinSeg_BasicInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
    return r;
}
