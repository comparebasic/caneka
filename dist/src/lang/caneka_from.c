#include <external.h>
#include <caneka.h>

Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    printf("Struct from called\n");
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);

    char *cstr = "";
    CnkLangModule *mod = CnkLangModule_Make(m); 
    Transp *ctx = (Transp*)as(fmt->source, TYPE_LANG_CNK);
    mod->cfile = ctx->current.dest;
    mod->name = (String *)asIfc(fmt->root->value, TYPE_STRING);

    mod->typeName = String_Init(m, STRING_EXTEND);
    cstr = "TYPE_";
    String_AddBytes(m, mod->typeName, bytes(cstr), strlen(cstr));
    String_Add(m, mod->typeName, mod->name);
    String_MakeUpper(mod->typeName);

    Iter it;
    while(item != NULL){
            
        CnkLangModule_SetItem(m, mod, item);
        if(item->children != NULL){
            Iter_Init(&it, item->children);
            if((Iter_Next(&it) & END) == 0){
                FmtItem *subItem = (FmtItem *)Iter_Get(&it);
                if(subItem != NULL && subItem->def != NULL && subItem->def->to != NULL){
                    CnkLangModule_SetItem(m, mod, subItem);
                }
            }
        }
        if(item->def->to != NULL){
            ;
        }
        item = NULL;
    }

    return (Abstract *)mod;
}

