#include <external.h>
#include <caneka.h>

String *PatChar_FromString(MemCtx *m, String *s){
    String *pat = String_Init(m, STRING_EXTEND);
    PatCharDef pdef;
    if(String_Next(s) != NULL || (s->length+1) * sizeof(PatCharDef) > STRING_CHUNK_SIZE){
        Fatal("Pat char not yet implemented for strings of this length", TYPE_STRING);
        return NULL;
    }
    for(int i = 0; i < s->length; i++){
        pdef.flags = PAT_TERM;
        pdef.from = (word)s->bytes[i];
        pdef.to = (word)s->bytes[i];
        String_AddBytes(m, pat, (byte *)&pdef, sizeof(PatCharDef));
    }
    pdef.flags = PAT_END;
    pdef.from = 0;
    pdef.to = 0;
    String_AddBytes(m, pat, (byte *)&pdef, sizeof(PatCharDef));

    return pat;
}

String *PatChar_KoFromString(MemCtx *m, String *s){
    String *pat = String_Init(m, STRING_EXTEND);
    PatCharDef pdef;
    if(String_Next(s) != NULL || (s->length+1) * sizeof(PatCharDef) > STRING_CHUNK_SIZE){
        Fatal("Pat char not yet implemented for strings of this length", TYPE_STRING);
        return NULL;
    }
    for(int i = 0; i < s->length; i++){
        pdef.flags = PAT_KO|PAT_KO_TERM;
        pdef.from = (word)s->bytes[i];
        pdef.to = (word)s->bytes[i];
        String_AddBytes(m, pat, (byte *)&pdef, sizeof(PatCharDef));
    }

    return pat;
}

