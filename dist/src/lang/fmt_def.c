#include <external.h>
#include <caneka.h>

void FmtDef_PushItem(FmtCtx *ctx, word captureKey, String *s, FmtDef *def){
    FmtItem *item =  FmtItem_Make(ctx->m, ctx);
    item->spaceIdx = captureKey;
    item->parent = ctx->item;
    item->def = def;
    item->content = String_Init(ctx->m, STRING_EXTEND);
    String_Add(ctx->m, item->content, s);

    ctx->item = item;
}

FmtDef *FmtDef_Make(MemCtx *m){
    FmtDef *fi =  MemCtx_Alloc(m, sizeof(FmtDef));
    fi->type.of = TYPE_FMT_DEF;
    return fi;
}
