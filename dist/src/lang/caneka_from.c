#include <external.h>
#include <caneka.h>

Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
    printf("Struct From %s\n", CnkLang_RangeToChars(item->spaceIdx));

    char *cstr = "";
    CnkLangModule *mod = CnkLangModule_Make(m); 
    mod->ref = CnkLangModRef_Make(m);

    Transp *tp = (Transp*)as(fmt->source, TYPE_TRANSP);
    mod->cfile = tp->current.dest;

    Debug_Print((void *)tp->current.ext, 0, "Ext: ", COLOR_PURPLE, TRUE);
    printf("\n");

    String *name_ = String_Clone(m, tp->current.fname);
    String_Trunc(name_, -(tp->current.ext->length));
    mod->ref->name = String_ToCamel(m, name_);

    mod->ref->typeName = String_Init(m, STRING_EXTEND);
    cstr = "TYPE_";
    String_AddBytes(m, mod->ref->typeName, bytes(cstr), strlen(cstr));
    String_Add(m, mod->ref->typeName, mod->ref->name);
    String_MakeUpper(mod->ref->typeName);

    Debug_Print((void *)item->children, 0, "Children:", COLOR_PURPLE, TRUE);
    printf("\n");

    if(item->children != NULL){
        Iter it;
        Iter_Init(&it, item->children);
        while((Iter_Next(&it) & END) == 0){
            Abstract *x = Iter_Get(&it);
            CnkLangModule_SetItem(m, mod, item, x);
        }
    }

    if(DEBUG_LANG_CNK){
        Debug_Print((void *)mod, 0, "Struct mod: ", DEBUG_LANG_CNK, TRUE);
        printf("\n");
    }

    return (Abstract *)mod;
}

