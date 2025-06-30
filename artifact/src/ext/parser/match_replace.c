#include <external.h>
#include <caneka.h>

status Match_StrReplace(MemCh *m, Str *s, Str *new, Match *mt, i32 *_pos){
    status r = READY;
    i64 pos = 0;
    while(pos < s->length && (Match_Feed(m, mt, s->bytes[pos]) & SUCCESS) == 0){
        pos++;
    }

    if(mt->type.state & SUCCESS){
        pos++;
        i32 start = pos - SnipSpan_Total(mt->backlog, ZERO);
        r |= SUCCESS;
        *_pos = pos;
        Iter it;
        Iter_Init(&it, mt->backlog);
        while((Iter_Next(&it) & END) == 0){
            Snip *sn = Iter_Get(&it);
            if(sn->type.state & SNIP_CONTENT){
                if(sn->length < new->length){
                    i32 delta = new->length - sn->length;
                    if(s->length + delta + 1 > s->alloc){
                        r |= ERROR;
                        return r;
                    }
                    i32 overlapStart = start+sn->length;
                    i64 sz = s->length-overlapStart;
                    memmove(s->bytes+start+new->length, s->bytes+overlapStart, sz);
                    s->length += delta;
                }else if(sn->length > new->length){
                    i32 overlapStart = start+sn->length;
                    i64 sz = s->length-overlapStart;
                    memmove(s->bytes+start+new->length, s->bytes+overlapStart, sz);
                    i32 delta = sn->length - new->length; 
                    memset(s->bytes+(s->length-delta), 0, delta);
                    s->length -= delta;
                }
                memcpy(s->bytes+start, new->bytes, new->length);
                break;
            }else{
                start += sn->length;
            }
        }
    }

    if(pos == s->length-1){
        r |= END;
    }else{
        r |= MORE;
    }
    return r;
}
