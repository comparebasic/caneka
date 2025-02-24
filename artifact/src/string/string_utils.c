#include <external.h>
#include <caneka.h>

String *String_Sub(MemCtx *m, String *s, i64 start, i64 end){
    String *ret = String_Init(m, STRING_EXTEND);
    String_AddSub(m, ret, s, start, end);
    return ret;
}

String *String_MakeFixed(MemCtx *m, byte *bytes, int length){
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);

    return s;
}

String *String_SubMatch(MemCtx *m, String *s, Match *mt){
    String *seg = s;
    while(seg != NULL){
        for(int i = 0; i < seg->length; i++){
            Match_Feed(m, mt, (word)seg->bytes[i]);
        }
        seg = String_Next(seg);
    };

    Match_FeedEnd(m, mt);
    if((mt->type.state & SUCCESS)){
        return StrSnipStr_ToString(m, mt->backlog, s);
    }
    return NULL;
}

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
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;

    while(s != NULL){
        Match_SetPattern(&mt, (PatCharDef *)pat->bytes, backlog); 
        for(int i = offset; i < s->length; i++){
            status mr = Match_Feed(m, &mt, (word)s->bytes[i]);
            if((mr & SUCCESS) != 0){
                String *arg = String_Sub(m, s, offset, mt.snip.length);

                Span_Add(p, (Abstract *)arg);
                offset += Match_Total(&mt);
                String_Reset(backlog);
                Match_SetPattern(&mt, (PatCharDef *)pat->bytes, backlog);
            }
        }
        status mr = Match_Feed(m, &mt, (word)'/');
        if((mr & SUCCESS) != 0){
            String *arg = String_Sub(m, s, offset, mt.snip.length);  

            Span_Add(p, (Abstract *)arg);
            offset += Match_Total(&mt);
            String_Reset(backlog);
            Match_SetPattern(&mt, (PatCharDef *)pat->bytes, backlog); 
        }

        s = String_Next(s);
    }

    return p;
}

