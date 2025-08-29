#include <external.h>
#include <caneka.h>

TemplCtx *TemplCtx_Make(MemCh *m, Cursor *curs, Abstract *source){
    TemplCtx *ctx = (TemplCtx *)MemCh_Alloc(m, sizeof(TemplCtx));
    ctx->type.of = TYPE_TEMPL_CTX;
    ctx->rbl = Templ_RoeblingMake(m, curs, (Abstract *)ctx);
    if(curs->type.state & DEBUG){
        ctx->rbl->type.state |= DEBUG;
        ctx->rbl->mess->type.state |= DEBUG;
    }
    Iter_Setup(&ctx->it, Span_Make(m), SPAN_OP_GET, 0);
    return ctx;
}

TemplCtx *TemplCtx_FromCurs(MemCh *m, Cursor *curs, Abstract *source){
    TemplCtx *ctx = TemplCtx_Make(m, curs, source);
    Roebling_Run(ctx->rbl);
    Roebling_Finalize(ctx->rbl, NULL, NEGATIVE);
    if((ctx->rbl->curs->type.state & END) && (ctx->rbl->type.state & ERROR) == 0){
        ctx->type.state |= SUCCESS;;
    }

    return ctx;
}
