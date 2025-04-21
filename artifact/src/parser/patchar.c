#include <external.h>
#include <caneka.h>

static Str *PatChar_fromStr(MemCh *m, Str *s, word flags){
    Str *pat = Str_Make(m, STR_DEFAULT);
    i32 max = (STR_DEFAULT / sizeof(PatCharDef))-1;
    if(s->length > max){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Str longer than pat char max", NULL);
        return NULL;
    }
    PatCharDef *pdef = (PatCharDef *)pat->bytes;
    for(i32 i = 0; i < s->length; i++, pdef++){
        pdef->flags = flags;
        pdef->from = pdef->to = s->bytes[i];
    }
    pdef->flags = PAT_END;
    pdef->from = 0;
    pdef->to = 0;

    return pat;
}

Str *PatChar_FromStr(MemCh *m, Str *s){
    return PatChar_fromStr(m, s, PAT_TERM);
}

Str *PatChar_KoFromStr(MemCh *m, Str *s){
    return PatChar_fromStr(m, s, (PAT_KO|PAT_KO_TERM));
}

