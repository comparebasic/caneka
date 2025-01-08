#include <external.h>
#include <caneka.h>

Span *String_SplitToSpan(MemCtx *m, String *s, String *sep){
    String *pat = PatChar_KoFromString(m, sep);
    PatCharDef pdef[] = {
       patTextDef 
    };
    String_AddBytes(m, pat, bytes(pdef), sizeof(pdef));
    PatCharDef pend[] = {
       {PAT_END, 0, 0} 
    };
    String_AddBytes(m, pat, bytes(pend), sizeof(pend));

    Span *p = Span_Make(m, TYPE_SPAN);

    i64 offset = 1;
    Match mt;

    while(s != NULL){
        Match_SetPattern(&mt, (PatCharDef *)pat->bytes); 
        for(int i = offset; i < s->length; i++){
            status mr = Match_Feed(&mt, (word)s->bytes[i]);
            if((mr & SUCCESS) != 0){
                String *arg = String_Sub(m, s, offset, mt.count);  

                Span_Add(p, (Abstract *)arg);
                offset += mt.count+mt.lead+mt.tail;
                Match_SetPattern(&mt, (PatCharDef *)pat->bytes); 
            }
        }
        status mr = Match_Feed(&mt, (word)'/');
        if((mr & SUCCESS) != 0){
            String *arg = String_Sub(m, s, offset, mt.count);  

            Span_Add(p, (Abstract *)arg);
            offset += mt.count+mt.lead+mt.tail;
            Match_SetPattern(&mt, (PatCharDef *)pat->bytes); 
        }

        s = String_Next(s);
    }

    return p;
}
