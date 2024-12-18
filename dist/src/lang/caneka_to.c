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

Abstract *CnkLang_StructTo(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a){
    printf("StructTo\n");
    CnkLangModule *mod = (CnkLangModule *)as(a, TYPE_LANG_CNK_MODULE);

    String *s = String_Init(m, STRING_EXTEND);
    String_Add(m, s, mod->name);
    char *cstr = " *";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->name);
    cstr = "_Make(MemCtx *m){\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    cstr = "    ";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->name);
    cstr = " *o = (";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->name);
    cstr = "*)MemCtx_Alloc(m, sizeof(";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->name);
    cstr = "));\n    o->type.of = ";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_Add(m, s, mod->typeName);

    cstr = ";\n    return o;\n}\n";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));

    fmt->out(m, s, fmt->source);

    return (Abstract *)s;
}
