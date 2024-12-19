#include <external.h>
#include <caneka.h>

CnkLangModRef *CnkLangModRef_Make(MemCtx *m){
    CnkLangModRef *r = MemCtx_Alloc(m, sizeof(CnkLangModRef));
    r->type.of = TYPE_LANG_CNK_MOD_REF;
    return r;
}

CnkLangModule *CnkLangModule_Make(MemCtx *m){
    CnkLangModule *mod = MemCtx_Alloc(m, sizeof(CnkLangModule));
    mod->type.of = TYPE_LANG_CNK_MODULE;
    mod->args = Span_Make(m, TYPE_SPAN);
    mod->funcDefs = Span_Make(m, TYPE_SPAN);
    return mod;
}

status CnkLangModule_SetItem(MemCtx *m, CnkLangModule *mod, FmtItem *item, Abstract *a){
    if(DEBUG_LANG_CNK){
        Debug_Print((void *)a, 0, "setting item: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }
    return NOOP; 
}
