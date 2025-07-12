#include <external.h>
#include <caneka.h>

CashCtx *CashCtx_Make(MemCh *m, Cursor *curs, Abstract *source){
    CashCtx *ctx = MemCh_Alloc(m, sizeof(CashCtx));
    ctx->type.of = TYPE_CASH_CTX;
    ctx->rbl = Cash_RoeblingMake(m, curs, (Abstract *)ctx);
    Iter_Setup(&ctx->it, Span_Make(m), SPAN_OP_GET, 0);
    return ctx;
}

CashCtx *CashCtx_FromCurs(MemCh *m, Cursor *curs, Abstract *source){
    CashCtx *ctx = CashCtx_Make(m, curs, source);

    if(curs->type.state & DEBUG){
        ctx->rbl->type.state |= DEBUG;
    }
    Roebling_Run(ctx->rbl);
    Roebling_Finalize(ctx->rbl, NULL, NEGATIVE);
    if((ctx->rbl->curs->type.state & END) && (ctx->rbl->type.state & ERROR) == 0){
        ctx->type.state |= SUCCESS;;
    }

    return ctx;
}
