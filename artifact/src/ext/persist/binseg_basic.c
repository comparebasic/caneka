#include <external.h>
#include <caneka.h>

static i64 Table_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    i64 total = 0;
    MemCh *m = ctx->bf->m;
    Table *tbl = (Span *)as(a, TYPE_TABLE);

    Str *content;
    Str *entry;
    BinSegHeader *hdr = BinSegHeader_Make(ctx,
        tbl->nvalues,
        BINSEG_TYPE_DICTIONARY,
        id,
        &content,
        &entry);

    i16 *ip = (i16 *)content->bytes;
    for(i32 i = 0; i < tbl->nvalues*2; i++){
        *ip = ctx->func(ctx, NULL);
        ip++;
    }

    if(ctx->type.state & BINSEG_REVERSED){
        total += BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    ip = (i16 *)content->bytes;
    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            total += BinSegCtx_Send(ctx, h->key, *(ip++));
            total += BinSegCtx_Send(ctx, h->value, *(ip++));
        }
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        total += BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return total;
}

static i64 Span_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    i64 total = 0;
    MemCh *m = ctx->bf->m;
    Span *p = (Span *)as(a, TYPE_SPAN);

    Str *content;
    Str *entry;
    BinSegHeader *hdr = BinSegHeader_Make(ctx,
        p->nvalues,
        BINSEG_TYPE_COLLECTION,
        id,
        &content,
        &entry);


    i16 *ip = (i16 *)content->bytes;
    for(i32 i = 0; i < p->nvalues; i++){
        *ip = ctx->func(ctx, NULL);
        ip++;
    }

    if(ctx->type.state & BINSEG_REVERSED){
        total += BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    ip = (i16 *)content->bytes;
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        total += BinSegCtx_Send(ctx, Iter_Get(&it), *ip);
        ip++;
    };


    if((ctx->type.state & BINSEG_REVERSED) == 0){
        total += BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return total;
}

static i64 Str_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    Str *s = (Str *)as(a, TYPE_STR);
    MemCh *m = ctx->bf->m;

    Str *content;
    Str *entry;
    BinSegHeader *hdr = BinSegHeader_Make(ctx,
        s->length,
        BINSEG_TYPE_BYTES,
        id,
        &content,
        &entry);

    memcpy(content->bytes, s->bytes, s->length);

    return BinSegCtx_ToBuff(ctx, hdr, entry);
}

status BinSeg_BasicInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_TABLE, (void *)Table_ToBinSeg);
    return r;
}
