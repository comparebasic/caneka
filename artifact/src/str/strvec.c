#include <external.h>
#include <caneka.h>

status StrVec_Copy(StrVec *from, StrVec *to, i64 start, i64 end){
    /* navigate to the start position */

    /* add the str objects to the to strvec: */

    /* decide if it's a continuation of the same string as new segments of 'from'
     * are encountered */
    return NOOP;
}

i64 StrVec_ToFd(StrVec *v, int fd){
    Iter it;
    Iter_Init(&it, v->p);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s != NULL){
            total += Str_ToFd(s, fd);
        }
    }
    return total;
}

Str *StrVec_ReAlign(MemCtx *m, StrVec *v){
    /* make long strings instead of short ones and re attach a new span to 'v' */
    return NULL;
}

status StrVec_NextSlot(StrVec *v, Cursor *curs){
    i64 offset = 0;
    util needed = 8;
    curs->slot = 0;
    do {
        if(curs->ptr == curs->end){
            if((Iter_Next(&curs->it) & END) != 0){
                return ERROR;
            }
            Str *s = (Str *)Iter_Get(&curs->it);
            curs->ptr = s->bytes;
            curs->end = s->bytes+s->length-1;
        }
        i64 remaining = ((curs->end) - curs->ptr);
        i64 len = min(remaining, needed);
        if(len == 0){
            break;
        }
        memcpy(((byte *)(&curs->slot))+offset, curs->ptr, len);
        curs->ptr += len;
        curs->pos += len;
        offset = curs->pos & 7;
        needed = 8 - offset;
    } while(offset != 0);

    curs->type.state |= SUCCESS;
    return curs->type.state;
}

status StrVec_Add(StrVec *v, Str *s){
    status r = Span_Add(v->p, (Abstract *)s);
    v->total += s->length;
    return r;
}

status StrVec_AddVec(StrVec *v, StrVec *v2){
    Iter it;
    Iter_Init(&it, v2->p);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s != NULL){
            StrVec_Add(v, s);
        }
    }
    return SUCCESS;
}

status StrVec_AddBytes(MemCtx *m, StrVec *v, byte *ptr, i64 length){
    Str *s = Span_Get(v->p, v->p->max_idx);
    status r = READY;
    while(length > 0){
        if(s == NULL){
            s = Str_Make(m, STR_DEFAULT);
            r |= StrVec_Add(v, s);
        }
        while(s->length < s->alloc && length > 0){
            i64 take = min((s->alloc - s->length), length);
            memcpy(s->bytes+s->length, ptr, take);
            length -= take;
            ptr += take;
            s->length += (word)take;
        }
        s = NULL;
    }
    return r;
}

StrVec *StrVec_Make(MemCtx *m){
    StrVec *v = MemCtx_Alloc(m, sizeof(StrVec));
    v->type.of = TYPE_STRVEC;
    Span *p = Span_Make(m);
    return v;
}
