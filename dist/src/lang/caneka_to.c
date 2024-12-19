#include <external.h>
#include <caneka.h>

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

Abstract *CnkLang_Start(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
    String *s = String_Init(m, STRING_EXTEND);
    char *cstr = "#include <external.h>\n#include <caneka.h>\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    cstr = "#include <";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String *hfile = String_Init(m, STRING_EXTEND);
    String_Add(m, hfile, (String *)item->value);
    String_MakeLower(hfile);
    cstr = ".h";
    String_AddBytes(m, hfile, bytes(cstr), strlen(cstr));
    String_Add(m, s, hfile);
    cstr = ">\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    return (Abstract *)s;
}

static void addArgs(MemCtx *m, CnkLangModule *mod, String *s){
    Iter it;;
    Iter_Init(&it, mod->args);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            CnkLangModRef *ref = (CnkLangModRef *)as(h->value, TYPE_LANG_CNK_MOD_REF);
            if(ref->spaceIdx == CNK_LANG_VALUE){
                String_AddBytes(m, s, bytes(", "), 2); 
                String_Add(m, s, ref->name); 
                String_AddBytes(m, s, bytes(" *"), 2); 
                String_Add(m, s, (String *)h->item); 
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
                char *cstr = "    o->";
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

Abstract *CnkLang_StructTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
    CnkLangModule *mod = (CnkLangModule *)as(item->value, TYPE_LANG_CNK_MODULE);

    String *s = String_Init(m, STRING_EXTEND);
    String_Add(m, s, mod->ref->name);
    char *cstr = " *";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->ref->name);
    cstr = "_Make(MemCtx *m";
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


    fmt->out(m, s, fmt->source);

    return (Abstract *)s;
}
