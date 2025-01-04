#include <external.h>
#include <caneka.h>

status Roebling_SyntaxTests(MemCtx *gm){
    Stack(bytes("Roebling_SyntaxTests"), NULL);

    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    FmtCtx *ctx = NULL;
    ctx = CnkRoeblingCtx_Make(m, NULL);

    /* phrase */
    s = String_Make(m, bytes("/(hi-there)/\n"));
    DebugStack_SetRef((Abstract *)s);

    /*
    Debug_Print((void *)s, 0, "Phrase:", COLOR_PURPLE, FALSE);
    printf("\n");
    */

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

    String *exp = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, exp, bytes(cstr), strlen(cstr));

    r |= Test(String_EqualsBytes(csource, bytes(cstr)), "Pattern hi-there produces expected C source code");

    /* line */
    s = String_Make(m, bytes("/strings(\\n -> ,ONE,TWO,THREE), line(.+[\\n]) -> strings/\n"));
    DebugStack_SetRef((Abstract *)s);

    Debug_Print((void *)s, 0, "Line:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);

    Return r;

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
    Return r;
}

