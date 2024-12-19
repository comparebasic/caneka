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
    mod->args = Span_Make(m, TYPE_TABLE);
    mod->funcDefs = Span_Make(m, TYPE_SPAN);
    return mod;
}

status CnkLangModule_SetItem(MemCtx *m, CnkLangModule *mod, FmtItem *item, Abstract *a){
    if(DEBUG_LANG_CNK){
        Debug_Print((void *)a, 0, "setting item: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

    if(Ifc_Match(a->type.of, TYPE_FMT_ITEM)){
        CnkLangModRef *ref = NULL;
        FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
        Result *second = (Result *)Span_Get(item->children, 0);
        Result *third = (Result *)Span_Get(item->children, 1);

        if(third != NULL && (third->type.of == TYPE_RESULT && second->type.of == TYPE_RESULT) && second->range == CNK_LANG_ASSIGN){
             ref = CnkLangModRef_Make(m); 
             ref->name = third->s;
             ref->spaceIdx = third->range;
        }

        Result *fourth = (Result *)Span_Get(item->children, 2);
        if(fourth != NULL){
             ref->next = CnkLangModRef_Make(m); 
             ref->next->name = fourth->s;
             ref->next->spaceIdx = fourth->range;
        }

        if(item->def->id == CNK_LANG_TOKEN){
            Table_Set(mod->args, (Abstract *)item->content, (Abstract *)ref);
        }
    }

    return NOOP; 
}
