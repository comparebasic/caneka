#include <external.h>
#include <caneka.h>

Lookup *BinSegLookup = NULL;

i32 BinSeg_IdxCounter(BinSegCtx *ctx, Abstract *arg){
    if(ctx->source == NULL){
        ctx->source = I32_Wrapped(ctx->sm->m, 0);
        return 0 
    }else{
        return ++((Single *)ctx->source)->val.i;
    }
}

BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source){
    MemCh *m = sm->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    if(func == NULL){
        ctx->func = BinSeg_IdxCounter;
    }

    ctx->cortext = Table_Make(m);
    ctx->source = source;
    return ctx;
}

status BinSeg_Init(MemCh *m, Lookup *lk){
    status r = READY;
    if(BinSegLookup == NULL){
        BinSegLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= BinSeg_BasicInit(m, BinSegLookup);
        r |= SUCCESS;
    }

    return r;
}
