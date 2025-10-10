#include <external.h>
#include <caneka.h>

static i64 Table_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    i64 total = 0;
    MemCh *m = ctx->sm->m;
    Table *tbl = (Span *)as(a, TYPE_TABLE);

    Str *s = Str_Make(m, sizeof(i16)*tbl->nvalues*2);
    s->length = s->alloc;
    i16 *ip = (i16 *)s->bytes;
    i16 i = 0;
    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            i16 keyId = ctx->func(ctx, NULL);
            i16 id = ctx->func(ctx, NULL);
            total += BinSegCtx_Send(ctx, h->key, keyId);
            total += BinSegCtx_Send(ctx, h->value, id);
            ip[i*2] = keyId;
            ip[i*2+1] = id;
            i++;
        }
    };

    BinSegHeader hdr = {
       .total = tbl->nvalues, 
       .kind = BINSEG_TYPE_DICTIONARY,
       .id = id,
    };

    total += BinSegCtx_ToStream(ctx, &hdr, s);
    return total;
}

static i64 Span_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    i64 total = 0;
    MemCh *m = ctx->sm->m;
    Span *p = (Span *)as(a, TYPE_SPAN);

    Str *s = Str_Make(m, sizeof(i16)*p->nvalues);
    s->length = s->alloc;
    i16 *ip = (i16 *)s->bytes;

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        i16 id = ctx->func(ctx, NULL);
        total += BinSegCtx_Send(ctx, Iter_Get(&it), id);
        *ip = id;
        ip++;
    };

    BinSegHeader hdr = {
       .total = p->nvalues, 
       .kind = BINSEG_TYPE_COLLECTION,
       .id = id,
    };

    total += BinSegCtx_ToStream(ctx, &hdr, s);
    return total;
}

static i64 Str_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    Str *s = (Str *)as(a, TYPE_STR);
    MemCh *m = ctx->sm->m;

    BinSegHeader hdr = {
       .total = s->length, 
       .kind = BINSEG_TYPE_BYTES,
       .id = id,
    };

    return BinSegCtx_ToStream(ctx, &hdr, s);
}

status BinSeg_BasicInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_TABLE, (void *)Table_ToBinSeg);
    return r;
}
