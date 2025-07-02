#include <external.h>
#include <caneka.h>

status StrVec_Split(StrVec *v, Abstract *split){
    v->m->type.range++;
    Match *mt = NULL;
    if(split->type.of == TYPE_WRAPPED_I8){
        Single *sg = (Single)split;
        mt = Match_Make(v->m, PatChar_FromStr(m, Str_Ref(m, &sg->val.b, 1, 2)), 
            Span_Make(v->m));
        mt->type.state |= MATCH_SEARCH;
    }else if(split->type.of == TYPE_MATCH){
        mt = (Match *)split;
    }else if(split->type.of == TYPE_STR){
        mt = Match_Make(v->m, PatChar_FromStr(m, (Str *s)split), Span_Make(v->m));
        mt->type.state |= MATCH_SEARCH;
    }
    if(mt == NULL){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, split->type.of),
            NULL
        };
        Error(ErrStream->m, v, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to split StrVec with type $", args);
    }

    Iter it;
    Iter_Init(&it, v->p);
    i32 taken = 0;
    while((Iter_Next(&it) & END) == 0){
        status r = READY;
        while((r & LAST) == 0){
            if(mt->type.state & PROCESSING){

            }
            r &= ~MORE;
            Str *s = (Str *)Iter_Get(&it);
            i32 pos = 0;
            while(pos < s->length && (Match_Feed(m, mt, s->bytes[pos]) & SUCCESS) == 0){
                pos++;
            }

            if(pos == s->length-1){
                r |= LAST;
            }else{
                r |= MORE;
            }

            if(mt->type.state & SUCCESS){
                i32 start = pos - SnipSpan_Total(mt->backlog, ZERO);
                while((Iter_Next(&it) & END) == 0){
                    Snip *sn = Iter_Get(&it);
                    if(sn->type.state & SNIP_CONTENT){
                        s->length = start;
                        i32 idx = it.idx;
                        Span_Insert(v->m, idx+1, (Abstract *)Str_Ref(v->m, s->bytes+start, sn->length, sn->length, STRING_SEPERATOR));
                        Iter_GetByIdx(it, idx+1);
                        if(r & MORE){
                            s = Str_Ref(v->m, s->bytes+start+sn->length, s->length-(start+sn->length));
                        }
                        break;
                    }else{
                        start += sn->length;
                    }
                }
                Match_StartOver(mt);
            }
        }

    }

    return NOOP;
}

Abstract *StrVec_Nth(MemCh *m, StrVec *v, i32 n){
    return NULL;
}

Span *StrVec_ToSpan(MemCh *m, StrVec *v){
    return NULL;
}
