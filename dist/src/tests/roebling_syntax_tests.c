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

    String *csource = (String *)asIfc(ctx->item->def->to(m, ctx->item->def, ctx, NULL, NULL), TYPE_STRING);

    char *cstr = "static PatCharDef _pat0[] = {\n"
    "    {PAT_TERM, 'h', 'h'},\n"
    "    {PAT_TERM, 'i', 'i'},\n"
    "    {PAT_TERM, '-', '-'},\n"
    "    {PAT_TERM, 't', 't'},\n"
    "    {PAT_TERM, 'h', 'h'},\n"
    "    {PAT_TERM, 'e', 'e'},\n"
    "    {PAT_TERM, 'r', 'r'},\n"
    "    {PAT_TERM, 'e', 'e'},\n"
    "    {PAT_END, 0, 0}\n"
    "};\n";

    String *exp = String_Make(m, bytes(cstr));

    csource->type.state |= DEBUG;
    r |= Test(String_EqualsBytes(csource, bytes(cstr)), "Pattern hi-there produces expected C source code, expected '%s', have '%s'", String_ToChars(m, exp), String_ToChars(m, csource));
    Debug_Print((void *)csource, 0, "csource:\n", COLOR_PURPLE, TRUE);
    printf("\n");
    Debug_Print((void *)exp, 0, "exp:\n", COLOR_PURPLE, TRUE);
    printf("\n");

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

