#include <external.h>
#include <caneka.h>

status StrVec_Split(StrVec *v, Abstract *split){
    i16 guard;
    status r = READY;
    MemCh *m = v->p->m;
    m->type.range++;
    Match *mt = NULL;
    if(split->type.of == TYPE_WRAPPED_I8){
        Single *sg = (Single *)split;
        mt = Match_Make(m, PatChar_FromStr(m, Str_Ref(m, &sg->val.b, 1, 1, ZERO)), 
            Span_Make(m));
        mt->type.state |= MATCH_SEARCH;
    }else if(split->type.of == TYPE_PATMATCH){
        mt = (Match *)split;
        Match_StartOver(mt);
    }else if(split->type.of == TYPE_STR){
        mt = Match_Make(m, PatChar_FromStr(m, (Str *)split), Span_Make(m));
        mt->type.state |= MATCH_SEARCH;
    }

    if(mt == NULL){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, split->type.of),
            NULL
        };
        Error(ErrStream->m, (Abstract *)v, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to split StrVec with type $", args);
    }

    Iter it;
    Iter_Init(&it, v->p);
    Str *taken = NULL;
    while((Iter_Next(&it) & END) == 0){
        status r = READY;
        Str *s = (Str *)Iter_Get(&it);
        i32 pos = 0;
        while((r & LAST) == 0){
            Guard_Incr(&guard, 100, FUNCNAME, FILENAME, LINENUMBER);
            r &= ~MORE;
            if(s == NULL){
                Abstract *args[] = {
                    (Abstract *)split,
                    (Abstract *)mt,
                    (Abstract *)v,
                    (Abstract *)s,
                    (Abstract *)&it,
                    NULL
                };
                Error(ErrStream->m, (Abstract *)v, FUNCNAME, FILENAME, LINENUMBER, 
                    "String not found in StrVec for split @ using & on & currenlty looking at & at &^0.\n", args);
            }

            pos = 0;
            while(pos < s->length && (Match_Feed(m, mt, s->bytes[pos]) & SUCCESS) == 0){
                pos++;
            }

            if(pos == s->length){
                r |= LAST;
            }else{
                r |= MORE;
            }

            if(mt->type.state & SUCCESS){
                i32 total = SnipSpan_Total(mt->backlog, ZERO);
                i32 start = pos - total;
                i32 takeTotal = total;
                if(taken != NULL){
                    takeTotal += taken->length;
                }
                if(takeTotal > STR_DEFAULT){
                    Abstract *args[] = {
                        (Abstract *)split,
                        NULL
                    };
                    Error(ErrStream->m, (Abstract *)v, FUNCNAME, FILENAME, LINENUMBER, "Match is longer than a single string, not yet supported &", args);
                    return ERROR;
                }
                Iter backlogIt; 
                Iter_Init(&backlogIt, mt->backlog);
                while((Iter_Next(&backlogIt) & END) == 0){
                    Snip *sn = Iter_Get(&backlogIt);
                    if(sn->type.state & SNIP_CONTENT){
                        i32 length = sn->length;
                        if(taken != NULL){
                            length -= taken->length;
                            taken = NULL;
                        }
                        m->type.range--;
                        Str *sep = Str_Ref(m, s->bytes+start+1, length, length, STRING_SEPERATOR);
                        r |= Iter_Insert(&it, it.idx+1, (Abstract *)sep);
                        m->type.range++;
                        i16 sLength = s->length;
                        s->length = (i16)start+1;
                        if(r & MORE && length > 0){
                            i16 newLength = sLength-(start+length+1);
                            m->type.range--;
                            s = Str_Ref(m, s->bytes+start+1+length, newLength, newLength, ZERO);
                            r |= Iter_Insert(&it, it.idx+1, (Abstract *)s);
                            m->type.range++;
                        }
                        break;
                    }else{
                        start += sn->length;
                    }
                }
                Match_StartOver(mt);
            }else if((r & LAST) && mt->type.state & PROCESSING && (mt->snip.type.state & SNIP_CONTENT) != 0){
                i32 start = pos - SnipSpan_Total(mt->backlog, ZERO);
                s->length = start;
                m->type.range--;
                taken = Str_Ref(m, s->bytes+start, mt->snip.length, mt->snip.length, STRING_SEPERATOR);
                Iter_Insert(&it, it.idx+1, (Abstract *)taken);
                m->type.range++;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    m->type.range++;
    MemCh_Free(m);
    m->type.range--;

    return r;
}

Span *Str_SplitToSpan(MemCh *m, Str *_s, Abstract *split, word flags){
    if(_s->length < 1 || ((flags & SPLIT_SKIP_FIRST_CHAR) && _s->length == 1)){
        return Span_Make(m);
    }
    StrVec *v = StrVec_Make(m);
    Str *s = Str_Clone(m, _s);
    if(flags & SPLIT_SKIP_FIRST_CHAR){
        Str_Incr(s, 1);
    }
    StrVec_Add(v, s);
    StrVec_Split(v, split);
    return StrVec_ToSpan(m, v);
}

Span *StrVec_SplitToSpan(MemCh *m, StrVec *_v, Abstract *split){
    StrVec *v = (StrVec *)StrVec_Clone(m, (Abstract *)_v);
    StrVec_Split(v, split);
    return StrVec_ToSpan(m, v);
}

Span *StrVec_ToSpan(MemCh *m, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    Span *p = Span_Make(m);
    StrVec *shelf = NULL;
    Str *s = NULL;
    Str *item = NULL;
    while((Iter_Next(&it) & END) == 0){
        Str *item = (Str *)Iter_Get(&it);
        if(item->type.state & STRING_SEPERATOR){
            if(shelf != NULL){
                Span_Add(p, (Abstract *)shelf);
                shelf = NULL;
            }else if(s != NULL){
                Span_Add(p, (Abstract *)s);
                s = NULL;
            }
        }else{
            if(shelf != NULL){
                StrVec_Add(shelf, s);
            }else if(s != NULL){
                shelf = StrVec_Make(m);
                StrVec_Add(shelf, s);
                s = NULL;
                StrVec_Add(shelf, item);
            }else{
                s = item;
            }
        }

        if((it.type.state & LAST) && (item->type.state & STRING_SEPERATOR) == 0){
            if(s != NULL){
                Span_Add(p, (Abstract *)s);
            }else if(shelf != NULL){
                printf("shelf != NULL\n");
                StrVec_Add(shelf, item);
                Span_Add(p, (Abstract *)shelf);
            }
        }
    }
    return p;
}
