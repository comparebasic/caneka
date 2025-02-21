#include <external.h>
#include <caneka.h>

void FmtDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    FmtDef *def = (FmtDef *)as(a, TYPE_FMT_DEF);
    printf("\x1b[%dm%sFmtDef<\x1b[1;%dm%s\x1b[%dm/%s id:%d %s>\x1b[0m", color, msg,
        color,
        String_ToChars(DebugM, def->name),
        color,
        String_ToChars(DebugM, def->alias),
        def->id,
        State_ToChars(def->type.state));
}

static void XmlCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    XmlCtx *ctx = (XmlCtx *)as(a, TYPE_XMLCTX);
    if(extended){
        printf("\x1b[%dm%sXmlCtx<%s nodes=", color, msg, State_ToChars(ctx->type.state));
        Debug_Print((void *)ctx->root, 0, "", color, FALSE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sXmlCtx<%s>\x1b[0m", color, msg, State_ToChars(ctx->type.state));
    }
}
