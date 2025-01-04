#include <external.h>
#include <caneka.h>

status Roebling_SyntaxTests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    FmtCtx *ctx = NULL;
    ctx = CnkRoeblingCtx_Make(m, NULL);

    /* phrase */
    s = String_Make(m, bytes("/(hi-there)/\n"));
    Debug_Print((void *)s, 0, "Phrase:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Add(ctx->rbl, s);
    Roebling_Run(ctx->rbl);

    r |= Test(ctx->item->spaceIdx == CNK_LANG_RBL_START, "ctx item is the root item which is the CNK_LANG_ROEBLING item");

    ctx->item->def->to(m, ctx->item->def, ctx, NULL, NULL);

    return r;

    /* line */
    s = String_Make(m, bytes("/({.*[\\n]})/\n"));
    Debug_Print((void *)s, 0, "Line:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);

    /* recurring line */
    s = String_Make(m, bytes("/line({.*[\\n]}) -> line/\n"));
    Debug_Print((void *)s, 0, "Recurring:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);

    /* http proto */
    s = String_Make(m, bytes(
        "/\n"
        "   method(GET,POST)\n"
        "   (\\s)\n" 
        "   path(.*[\\s])\n"
        "   proto(HTTP\\/1.{1-2})\n"
        "/\n"
    ));
    Debug_Print((void *)s, 0, "HttpProto:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

