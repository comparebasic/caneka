#include <external.h>
#include <caneka.h>

Abstract *CnkLang_StartTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    if((fmt->type.state & FMT_CTX_ENCOUNTER) != 0){
        Transp *tp = (Transp *)asIfc(fmt->source, TYPE_TRANSP);

        String *s = String_Init(m, STRING_EXTEND);
        char *cstr = "#include <external.h>\n#include <caneka.h>\n\n";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
        
        if(DEBUG_LANG_CNK_OUT){
            Debug_Print((void *)s, 0, "", DEBUG_LANG_CNK_OUT, TRUE);
            printf("\n");
        }

        Transp_Out(tp, s);

        return (Abstract *)s;
    }else{
        return NULL;
    }
}

Abstract *CnkLang_RequireTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    Span *p = (Span *)fmt->item->value;
    if(p == NULL || !Ifc_Match(p->type.of, TYPE_SPAN)){
        return NULL;
    }
    String *out = String_Init(m, STRING_EXTEND);
    char *cstr = "#include <";
    String_AddBytes(m, out, bytes(cstr), strlen(cstr));
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        String *s = (String *)Iter_Get(&it);
        if(s != NULL){
            String_Add(m, out, s);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                String_AddBytes(m, out, bytes("/"), 1);
            }else{
                String_AddBytes(m, out, bytes(".h>\n"), 4);
            }
        }
    }
    String_MakeLower(out);
    return (Abstract *)out;
}

static void addArgs(MemCtx *m, CnkLangModule *mod, String *s){
    Iter it;;
    Iter_Init(&it, mod->args);
    Span *modules = CnkLang_GetModules(m);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            CnkLangModRef *ref = (CnkLangModRef *)as(h->value, TYPE_LANG_CNK_MOD_REF);
            if(ref->spaceIdx == CNK_LANG_VALUE){

                CnkLangModule *o = (CnkLangModule *)Table_Get(modules, (Abstract *)ref->name);
                String_Add(m, s, ref->name); 
                if(o != NULL && (o->type.state & CNK_LANG_MOD_FUNC_PTR) != 0){
                    String_AddBytes(m, s, bytes(" "), 1); 
                }else{
                    String_AddBytes(m, s, bytes(" *"), 2); 
                }
                String_Add(m, s, (String *)h->item); 
                if((it.type.state & FLAG_ITER_LAST) == 0){
                    String_AddBytes(m, s, bytes(", "), 2); 
                }
            }
        }
    }
}

static void addInst(MemCtx *m, CnkLangModule *mod, String *s){
    Iter it;;
    Iter_Init(&it, mod->args);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            CnkLangModRef *ref = (CnkLangModRef *)as(h->value, TYPE_LANG_CNK_MOD_REF);
            if(ref->spaceIdx == CNK_LANG_VALUE){
                char *cstr = "    o->";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                String_Add(m, s, (String *)h->item); 
                String_AddBytes(m, s, bytes(" = "), 3); 
                String_Add(m, s, (String *)h->item); 
                String_AddBytes(m, s, bytes(";\n"), 2); 
            }else if(ref->spaceIdx == CNK_LANG_ROEBLING){
                char *cstr = "    /* Roebling = /";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                String_Add(m, s, ref->name); 
                cstr = "/ */\n";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                cstr = "    o->";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                String_Add(m, s, (String *)h->item); 
                String_AddBytes(m, s, bytes(" = "), 3); 
                String_Add(m, s, (String *)mod->ref->name); 
                cstr = "_RblMake_";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                String_Add(m, s, (String *)h->item); 
                cstr = "(m, (Abstract *)o, ";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                String_Add(m, s, (String *)mod->ref->name); 
                if(ref->next != NULL){
                    cstr = "_";
                    String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                    String_Add(m, s, (String *)ref->next->name); 
                    cstr = ");\n";
                    String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                }else{
                    cstr = "_Capture);\n";
                    String_AddBytes(m, s, bytes(cstr), strlen(cstr)); 
                }
            }
        }
    }
}

Abstract *CnkLang_StructFrom(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);

    char *cstr = "";
    CnkLangModule *mod = CnkLangModule_Make(m); 
    mod->ref = CnkLangModRef_Make(m);

    Transp *tp = (Transp*)as(fmt->source, TYPE_TRANSP);
    mod->cfile = tp->current.dest;

    String *name_ = String_Clone(m, tp->current.fname);

    String_Trunc(name_, -(tp->current.ext->length));

    mod->ref->name = String_ToCamel(m, name_);

    mod->ref->typeName = String_Init(m, STRING_EXTEND);
    cstr = "TYPE_";
    String_AddBytes(m, mod->ref->typeName, bytes(cstr), strlen(cstr));
    String_Add(m, mod->ref->typeName, mod->ref->name);
    String_MakeUpper(mod->ref->typeName);

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

    item->value = (Abstract *)mod;

    return (Abstract *)mod;
}



Abstract *CnkLang_StructTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    if(fmt->type.state & FMT_CTX_CHILDREN_DONE){
        CnkLang_StructFrom(m, def, fmt, key, a);
    }else{
        return NULL;
    }

    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
    CnkLangModule *mod = (CnkLangModule *)as(item->value, TYPE_LANG_CNK_MODULE);

    String *s = String_Init(m, STRING_EXTEND);
    String_Add(m, s, mod->ref->name);
    char *cstr = " *";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->name);

    cstr = "_Make(";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));

    addArgs(m, mod, s);

    cstr = "){\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    cstr = "    ";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->name);
    cstr = " *o = (";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->name);
    cstr = "*)MemCtx_Alloc(m, sizeof(";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->name);
    cstr = "));\n    o->type.of = ";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->typeName);
    cstr = ";\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));

    addInst(m, mod, s);

    cstr = "\n    return o;\n}\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    Transp *tp = (Transp *)asIfc(fmt->source, TYPE_TRANSP);
    
    if(DEBUG_LANG_CNK_OUT){
        Debug_Print((void *)s, 0, "", DEBUG_LANG_CNK_OUT, TRUE);
        printf("\n");
    }

    Transp_Out(tp, s);

    return (Abstract *)s;
}
