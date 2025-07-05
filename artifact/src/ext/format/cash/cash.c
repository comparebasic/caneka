#include <external.h>
#include <caneka.h>

status CashCtx_StreamTo(Stream *sm, byte *b, i32 length){
    Stream_Bytes(sm, b, length); 
    CashCtx *ctx = (CashCtx *)as(sm->source, TYPE_CASH_CTX);
    return Roebling_Run(ctx->rbl);
}

CashCtx *CashCtx_Make(MemCh *m, Stream *sm,  Abstract *source){
    CashCtx *ctx = (CashCtx *)(CashCtx*)MemCh_Alloc(m, sizeof(CashCtx));
    ctx->type.of = TYPE_CASH_CTX;
    ctx->rbl = Cash_RoeblingMake(m, sm->dest.curs, source);

    return ctx;
}
