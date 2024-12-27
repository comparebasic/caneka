#include <external.h>
#include <caneka.h>

status Roebling_SyntaxTests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    FmtCtx *ctx = NULL;

    /* phrase */
    s = String_Make(m, bytes("/hi there/"));
    Debug_Print((void *)s, 0, "Phrase:", COLOR_PURPLE, FALSE);
    printf("\n");

    ctx = CnkRoeblingCtx_Make(m, NULL);
    Roebling_Add(ctx->rbl, s);
    Roebling_RunCycle(ctx->rbl);

    /* line */
    s = String_Make(m, bytes("/{.*[\\n]}/"));
    Debug_Print((void *)s, 0, "Line:", COLOR_PURPLE, FALSE);
    printf("\n");

    /* recurring line */
    s = String_Make(m, bytes("/line({.*[\\n]}) -> line/"));
    Debug_Print((void *)s, 0, "Recurring:", COLOR_PURPLE, FALSE);
    printf("\n");

    /* http proto */
    s = String_Make(m, bytes(
        "/\n"
        "   method(GET,POST)\n"
        "   \\s\n" 
        "   path(.*[\\s])\n"
        "   proto(HTTP/1.{1-2})\n"
        "/"
    ));
    Debug_Print((void *)s, 0, "HttpProto:", COLOR_PURPLE, FALSE);
    printf("\n");

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

