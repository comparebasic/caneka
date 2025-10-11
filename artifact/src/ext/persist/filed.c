#include <external.h>
#include <caneka.h>


BinSegCtx *FileD_Open(MemCh *m, File *f){
    BinSegCtx *ctx = BinSegCtx_Make(f->sm, NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));
    BinSegCtx_Start(ctx);
    return ctx;
}

status FileD_Add(BinSegCtx *ctx, i16 id, Abstract *a){
    BinSegCtx_Send(ctx, a, ctx->func(ctx, NULL));
    return ctx->type.state;
}

Table *FileD_ToTbl(MemCh *ctx, File *f, Table *keys){
    BinSegCtx *ctx = BinSegCtx_Make(f->sm, NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));
    BinSegCtx_Start(ctx);
    ctx->keys = keys;
    BinSegCtx_LoadStream(ctx);
    return ctx->tbl;
}
