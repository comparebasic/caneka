#include <external.h>
#include <caneka.h>
#include <rblsh.h>

static void RblShCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    RblShCtx *ctx = (RblShCtx *)as(a, TYPE_RBLSH_CTX);
    printf("\x1b[%dmRblShCtx<>\x1b[0m", color);
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_RBLSH_CTX, (void *)RblShCtx_Print);
    return r;
}

status RblShDebug_Init(MemCtx *m){
    Lookup *funcs = Lookup_Make(m, _TYPE_APPS_START, populateDebugPrint, NULL);
    return Chain_Extend(m, DebugPrintChain, funcs);
}
