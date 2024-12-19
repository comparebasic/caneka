#include <external.h>
#include <caneka.h>

Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);

    char *cstr = "";
    CnkLangModule *mod = CnkLangModule_Make(m); 
    Transp *tp = (Transp*)as(fmt->source, TYPE_TRANSP);
    mod->cfile = tp->current.dest;

    Debug_Print((void *)tp->current.ext, 0, "Ext: ", COLOR_PURPLE, TRUE);
    printf("\n");

    String *name_ = String_Clone(m, tp->current.fname);
    String_Trunc(name_, -(tp->current.ext->length));
    mod->name = String_ToCamel(m, name_);

    mod->typeName = String_Init(m, STRING_EXTEND);
    cstr = "TYPE_";
    String_AddBytes(m, mod->typeName, bytes(cstr), strlen(cstr));
    String_Add(m, mod->typeName, mod->name);
    String_MakeUpper(mod->typeName);

    Debug_Print((void *)item->children, 0, "Children:", COLOR_PURPLE, TRUE);
    printf("\n");

    if(item->children != NULL){
        Iter it;
        Iter_Init(&it, item->children);
        while((Iter_Next(&it) & END) == 0){
            Abstract *x = Iter_Get(&it);
            Debug_Print((void *)x, 0, "Child", COLOR_PURPLE, TRUE);
            printf("\n");
            if(Ifc_Match(x->type.of, TYPE_FMT_ITEM)){
                FmtItem *subItem = (FmtItem *)x;
                if(subItem != NULL && subItem->def != NULL && subItem->def->to != NULL){
                    CnkLangModule_SetItem(m, mod, subItem);
                }
            }
        }
    }

    if(DEBUG_LANG_CNK){
        Debug_Print((void *)mod, 0, "Struct mod: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

    return (Abstract *)mod;
}

