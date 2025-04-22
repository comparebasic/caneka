#include <external.h>
#include <caneka.h>

static i64 PatChar_continueFromStr(MemCh *m, Str *s, Str *ps, word flags){
    byte *b = s->bytes;
    byte *end = s->bytes+(s->length-1);
    i64 total = 0;
    while(b <= end){
        PatCharDef *pat = (PatCharDef *)(ps->bytes+ps->length);
        pat->flags = flags;
        pat->from = pat->to = *b;
        b++;
        ps->length += sizeof(PatCharDef);
        total += sizeof(PatCharDef);
    }

    return total;
}

PatCharDef *PatChar_FromStrVec(MemCh *m, StrVec *v){
    size_t sz = (v->total+1) * sizeof(PatCharDef);
    if(sz > STR_MAX){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "StrVec too long for PatChar Str", NULL);
        return NULL;
    }
    Str *ps = Str_Make(m, sz);

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        PatChar_continueFromStr(m, s, ps, PAT_TERM);
    }
    
    PatCharDef *pat = (PatCharDef *)(ps->bytes+ps->length);
    pat->flags = PAT_END;
    pat->from = 0;
    pat->to = 0;
    ps->length += sizeof(PatCharDef);

    return (PatCharDef *)ps->bytes;
}

PatCharDef *PatChar_fromStr(MemCh *m, Str *s, word flags){
    size_t sz = (s->length+1) * sizeof(PatCharDef);
    if(sz > STR_MAX){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "Str too long for PatChar Str", NULL);
        return NULL;
    }
    Str *ps = Str_Make(m, sz);
    PatChar_continueFromStr(m, s, ps, flags);
    PatCharDef *pat = (PatCharDef *)(ps->bytes+ps->length);
    pat->flags = PAT_END;
    pat->from = 0;
    pat->to = 0;
    ps->length += sizeof(PatCharDef);
    return (PatCharDef *)ps->bytes;
}

PatCharDef *PatChar_FromStr(MemCh *m, Str *s){
    return PatChar_fromStr(m, s, PAT_TERM);
}

PatCharDef *PatChar_KoFromStr(MemCh *m, Str *s){
    return PatChar_fromStr(m, s, PAT_KO|PAT_KO_TERM);
}
