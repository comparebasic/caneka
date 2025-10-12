#include <external.h>
#include <caneka.h>

BinSegCtx *FileD_Open(File *f){
    if((f->type.state & PROCESSING) == 0 && (File_Open(f) & PROCESSING) == 0){
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening File for FileD", NULL);
        return NULL;
    }
    BinSegCtx *ctx = BinSegCtx_Make(f->sm, NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));
    BinSegCtx_Start(ctx);
    return ctx;
}

status FileD_Add(BinSegCtx *ctx, i16 id, Abstract *a){
    BinSegCtx_Send(ctx, a, ctx->func(ctx, NULL));
    return ctx->type.state;
}

Table *FileD_ToTbl(BinSegCtx *ctx, Table *keys){
    ctx->keys = keys;
    BinSegCtx_LoadStream(ctx);
    return ctx->tbl;
}
