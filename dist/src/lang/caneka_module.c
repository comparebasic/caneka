#include <external.h>
#include <caneka.h>

CnkLangModule *CnkLangModule_Make(MemCtx *m){
    CnkLangModule *mod = MemCtx_Alloc(m, sizeof(CnkLangModule));
    mod->type.of = TYPE_LANG_CNK_MODULE;
    mod->args = Span_Make(m, TYPE_SPAN);
    mod->funcDefs = Span_Make(m, TYPE_SPAN);
    return mod;
}

status CnkLangModule_SetItem(MemCtx *m, CnkLangModule *mod, FmtItem *item){
    return NOOP; 
}
