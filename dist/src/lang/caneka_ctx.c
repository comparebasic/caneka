#include <external.h>
#include <caneka.h>

static void pushItem(FmtCtx *ctx, word captureKey, FmtDef *def){
    FmtItem *item =  FmtItem_Make(ctx->m, ctx);
    item->spaceIdx = captureKey;
    item->parent = ctx->item;
    item->def = def;

    ctx->item = item;
}

static status Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_FMT_CTX);

    FmtDef *def = Chain_Get(ctx->byId, captureKey);
    if(def == NULL){
        def = TableChain_Get(ctx->byAlias, s);
    }
    if(def == NULL || (def->type.state & FMT_FL_TAXONAMY) != 0){
        def = Chain_Get(ctx->byId, captureKey);
    }

    if(captureKey == CNK_LANG_INVOKE){
        if(ctx->item != NULL && ctx->item->parent != NULL){
            if(ctx->item->children == NULL){
                ctx->item->children = Span_Make(ctx->m, TYPE_SPAN);
            }
            Span_Add(ctx->item->parent->children, (Abstract *)ctx->item);
        }

        def = Chain_Get(ctx->byId, CNK_LANG_ARG_LIST);
    }

    if(DEBUG_LANG_CNK){
        printf("\x1b[%dmCnk Capture %s:", DEBUG_LANG_CNK, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " - ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        Debug_Print((void *)def, 0, "def: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }
    if(ctx->item != NULL && ctx->item->parent != NULL){
        if(def != NULL && ((def->type.state & FMT_DEF_OUTDENT) != 0)){
            FmtItem *item = ctx->item;
            while(item->parent != NULL && (item->def->type.state & FMT_DEF_PARENT_ON_PARENT) != 0){
                printf("Outdenting from %s to %s\n", CnkLang_RangeToChars(ctx->item->spaceIdx), CnkLang_RangeToChars(ctx->item->parent->spaceIdx));
                item = item->parent;
                if(item->def->from != NULL){
                    item->value = item->def->from(ctx->m,
                        item->def, ctx, item->key, (Abstract *)item);
                }
                if(item->def->to != NULL){
                    item->def->to(ctx->m,
                        item->def, ctx, item->key, (Abstract *)item);
                }
            }
            ctx->item = item;
        }
    }


    if(def != NULL){
        if((def->type.state & FMT_DEF_INDENT) != 0){
            pushItem(ctx, captureKey, def);
            printf("Indenting %s from %s\n", CnkLang_RangeToChars(ctx->item->spaceIdx), CnkLang_RangeToChars(ctx->item->parent->spaceIdx));
            printf("Adding %s to %s\n", CnkLang_RangeToChars(ctx->item->spaceIdx), CnkLang_RangeToChars(ctx->item->parent->spaceIdx));
            Span_Add(FmtItem_GetChildren(ctx->m, ctx->item->parent), (Abstract *)ctx->item);
        }else if((def->type.state & FMT_DEF_OUTDENT) == 0){
            Abstract *child = (Abstract *)Result_Make(ctx->m, captureKey, s, source);
            printf("Adding %s/%s to %s\n", String_ToChars(ctx->m, s), CnkLang_RangeToChars(captureKey), CnkLang_RangeToChars(ctx->item->spaceIdx));
            Span_Add(FmtItem_GetChildren(ctx->m, ctx->item), child);
        }
    }

    if(DEBUG_LANG_CNK){
        Debug_Print((void *)ctx->item->def, 0, "Base FmtDef: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
        printf("\n");
    }

    if(captureKey == CNK_LANG_LINE_END || captureKey == CNK_LANG_CURLY_CLOSE){
        Roebling_JumpTo(ctx->rbl, ctx->item->spaceIdx);
    }

    return SUCCESS;
}

FmtCtx *CnkLangCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK;
    ctx->m = m;
    ctx->rbl = CnkLangCtx_RblMake(m, ctx, Capture);
    ctx->out = ToStdOut;

    ctx->rangeToChars = CnkLang_RangeToChars;
    CnkLang_AddDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->item->spaceIdx = CNK_LANG_START;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;

    return ctx;
}
