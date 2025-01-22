#include <external.h>
#include <caneka.h>

status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    DebugStack_Push("CnkRoebling_Capture"); 
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_LANG_CNK_RBL);

    if(DEBUG_LANG_CNK_RBL){
        printf("\n\n\x1b[%dmCnkRbl Capture %s:", DEBUG_LANG_CNK_RBL, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " ", DEBUG_LANG_CNK_RBL, TRUE);
        printf("\n");
        Debug_Print((void *)ctx->item->def, 0, "item->def: ", DEBUG_LANG_CNK_RBL, TRUE);
        printf("\x1b[%dm(%s)\x1b[0m\n", DEBUG_LANG_CNK_RBL, CnkLang_RangeToChars(ctx->item->def->id));
    }

    FmtDef *def = Chain_Get(ctx->resolver->byId, captureKey);
    if(def != NULL && def->to){
        if((def->flags & FMT_DEF_INDENT) != 0){
            FmtDef_PushItem(ctx, captureKey, s, def);
        }
        if(def->to != NULL && (def->type.state & FMT_DEF_TO_ON_CLOSE) == 0){
            def->to(ctx->rbl->m, def, ctx, s, source);
        }
        if((def->flags & FMT_DEF_OUTDENT) != 0){
            ctx->item = ctx->item->parent;
            if((def->to != NULL) && (def->type.state & FMT_DEF_TO_ON_CLOSE) != 0){
                def->to(ctx->rbl->m, def, ctx, s, source);
            }
        }
    }

    DebugStack_Pop();
    return NOOP;
}

