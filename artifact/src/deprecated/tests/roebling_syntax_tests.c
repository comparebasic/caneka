#include <external.h>
#include <caneka.h>

status Roebling_SyntaxTests(MemCtx *gm){
    DebugStack_Push(NULL, 0); 
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    FmtCtx *ctx = NULL;
    ctx = CnkRoeblingCtx_Make(m, NULL);

    /* phrase */
    s = String_Make(m, bytes("/(hi-there)/\n"));

    DebugStack_SetRef(s, s->type.of);

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
    if(r & ERROR){
        printf("\x1b[%dmr = %s\n'%s'\nvs\n'%s'\x1b[0m\n", COLOR_RED, State_ToChars(r), String_ToChars(DebugM, csource), cstr);
    }

    /* line
    s = String_Make(m, bytes("/type(\\n -> ,ONE,TWO,THREE), ( *), msg(.+[\\n]) -> type/\n"));

    Debug_Print((void *)s, 0, "Line:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);
    */

    /* recurring line
    s = String_Make(m, bytes("/line({.*[\\n]}) -> line/\n"));
    Debug_Print((void *)s, 0, "Recurring:", COLOR_PURPLE, FALSE);
    printf("\n");

    Roebling_Reset(m, ctx->rbl, s);
    Roebling_Run(ctx->rbl);
    */

    /* http proto 
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
    */

    MemCtx_Free(m);

    r |= SUCCESS;
    DebugStack_Pop();
    return r;
}

