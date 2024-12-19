#include <external.h>
#include <caneka.h>

static void CnkModule_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    CnkLangModule *mod = (CnkLangModule *)as(a, TYPE_LANG_CNK_MODULE);
    printf("\x1b[%dm%sCnkModule<%s", color, msg, CnkLang_RangeToChars(mod->spaceIdx));
    Debug_Print((void *)mod->name, 0, " name:", color, FALSE);
    Debug_Print((void *)mod->typeName, 0, " typeName:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->args, 0, "args:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->states, 0, "states:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->hfile, 0, "hfile:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->cfile, 0, "cfile:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->funcDefs, 0, "funcDefs:", color, FALSE);
    printf("\x1b[%dm>\x1b[0m\n", color);

}

static status populatePrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LANG_CNK_MODULE, (void *)CnkModule_Print);
    return r;
}

static Lookup *funcs = NULL;
status CnkLang_Init(MemCtx *m){
    if(funcs == NULL){
        funcs = Lookup_Make(m, CNK_LANG_START, populatePrint, NULL);
        return Chain_Extend(m, DebugPrintChain, funcs);
    }
    return NOOP;
}
