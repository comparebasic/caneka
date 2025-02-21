#include <external.h>
#include <caneka.h>

String *String_Sub(MemCtx *m, String *s, i64 start, i64 end){
    String *ret = String_Init(m, STRING_EXTEND);
    i64 pos = 0;
    while(s != NULL && pos + s->length < start){
        pos += s->length;
        s = String_Next(s);
    }

    start -= pos;
    int length = min(s->length-start, end-pos);
    while(s != NULL && length > 0){
        int length = min(s->length-start, end-pos);
        String_AddBytes(m, ret, s->bytes+start, length);
        pos += length;
        s = String_Next(s); 
        start = 0;
    }

    return ret;
}

String *String_MakeFixed(MemCtx *m, byte *bytes, int length){
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);

    return s;
}



status String_Trunc(String *s, i64 len){
    DebugStack_Push("String_Trunc", TYPE_CSTR);
    if(len < 0){
        len = String_Length(s) + len;
    }
    i64 actual = 0;
    String *tail = s;
    size_t sz = String_GetSegSize(s);
    while(tail != NULL){
        if(actual+tail->length > len){
            tail->length = len - actual;
            memset(tail->bytes+tail->length, 0, sz-tail->length);
            if(String_Next(s) != NULL){
                tail->next = NULL;
            }
            DebugStack_Pop();
            return SUCCESS;
        }
        actual += tail->length; 
        tail = String_Next(s); 
    };

    DebugStack_Pop();
    return NOOP;
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

status String_Reset(String *s){
    status r = NOOP;
    while(s != NULL){
        if(s->length > 0){
            memset(s->bytes, 0, s->length);
            r |= SUCCESS;
        }
        s->length = 0;
        s = String_Next(s);
    }
    return r;
}

