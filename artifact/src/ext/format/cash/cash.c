#include <external.h>
#include <caneka.h>

status CashCtx_StreamTo(Stream *sm, byte *b, i32 length){
    Stream_Bytes(sm, b, length); 
    CashCtx *ctx = as(sm->source, TYPE_CASH_CTX);
    return Roebling_Run(ctx->rbl);
}

Cash *Cash_Make(MemCtx *m, CashCtx *ctx, Abstract *source){
    Cash *o = (Cash*)MemCtx_Alloc(m, sizeof(Cash));
    o->type.of = TYPE_CASH;
    ctx->cash = o;
    return o;
}

CashCtx *CashCtx_Make(MemCtx *m, Stream *sm,  Abstract *source){
    CashCtx *ctx = (CashCtx*)MemCh_Alloc(m, sizeof(CashCtx));
    ctx->type.of = TYPE_CASH_CTX;
    ctx->rbl = Cash_RoeblingMake(m, sm->dest.curs, source);

    return ctx;
}
