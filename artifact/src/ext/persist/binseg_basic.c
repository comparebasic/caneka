#include <external.h>
#include <caneka.h>

static status Table_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    MemCh *m = ctx->bf->m;
    Table *tbl = (Span *)as(a, TYPE_TABLE);
    status r = READY;

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
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    ip = (i16 *)content->bytes;
    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            r |= BinSegCtx_Send(ctx, h->key, *(ip++));
            r |= BinSegCtx_Send(ctx, h->value, *(ip++));
        }
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return r;
}

static status Span_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    MemCh *m = ctx->bf->m;
    Span *p = (Span *)as(a, TYPE_SPAN);
    status r = READY;

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
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    ip = (i16 *)content->bytes;
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_Send(ctx, Iter_Get(&it), *ip);
        ip++;
    };


    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return r;
}

static status Str_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
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

static status StrVec_ToBinSeg(BinSegCtx *ctx, Abstract *a, i16 id){
    StrVec *v = (StrVec *)as(a, TYPE_STRVEC);
    MemCh *m = ctx->bf->m;
    status r = READY;

    Str *content;
    Str *entry;
    BinSegHeader *hdr = BinSegHeader_Make(ctx,
        v->p->nvalues,
        BINSEG_TYPE_BYTES_COLLECTION,
        id,
        &content,
        &entry);

    i16 *ip = (i16 *)content->bytes;
    for(i32 i = 0; i < v->p->nvalues; i++){
        *ip = ctx->func(ctx, NULL);
        ip++;
    }

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    ip = (i16 *)content->bytes;
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_Send(ctx, Iter_Get(&it), *ip);
        ip++;
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_ToBuff(ctx, hdr, entry);
    }

    return r;
}

status BinSeg_BasicInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_TABLE, (void *)Table_ToBinSeg);
    r |= Lookup_Add(m, BinSegLookup, TYPE_STRVEC, (void *)StrVec_ToBinSeg);
    return r;
}
