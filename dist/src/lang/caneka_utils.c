#include <external.h>
#include <caneka.h>

String *CnkLang_requireFromSpan(MemCtx *m, Span *p){
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
    return out;
}
