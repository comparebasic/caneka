#include <external.h>
#include <caneka.h>

StrVecCursor _strVecCurs;
StrVecCursor _strVecCursB;

StrVec *StrVec_Snip(MemCtx *m, Span *sns, StrVec *v){
    DebugStack_Push(sns, TYPE_STRSNIP_STRING);
    StrVec *ret = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, sns);
    i32 pos = 0;
    Iter_Reset(&v->it); 
    while((Iter_Next(&it) & END) == 0){
        StrSnip sn = (StrSnip)((util)Iter_Get(&it));
        if(s.type.state & STRSNIP_CONTENT){
            StrVec_Copy(v, ret, pos, pos+sn.length);
        }
        pos += sn.length;
    }

    return ret;
}

status StrVec_Copy(StrVec *from, StrVec *to, i64 start, i64 end){
    return NOOP;
}

i64 StrVec_ToFd(StrVec *v, int fd){
    Iter *it = &v->it;
    Iter_Reset(it);
    i64 total = 0;
    while((Iter_Next(it) & END) == 0){
        Str *s = (Str *)Iter_Get(it);
        if(s != NULL){
            total += Str_ToFd(s, fd);
        }
    }
    return total;
}

Str *StrVec_ToStr(MemCtx *m, StrVec *v){
    return NULL;
}

status StrVecCurs_Setup(StrVec *v, StrVecCursor *curs){
    curs->type.of = TYPE_STRVEC_CURS;
    curs->type.state = READY;
    curs->total = 0;
    curs->idx = 0;
    curs->ptr = NULL;
    curs->end = NULL;
    curs->slot = 0;
    Iter_Reset(&v->it);
    return SUCCESS;
}

status StrVec_NextSlot(StrVec *v, StrVecCursor *curs){
    i64 offset = 0;
    util needed = 8;
    curs->slot = 0;
    do {
        if(curs->ptr == curs->end){
            if((Iter_Next(&v->it) & END) != 0){
                return ERROR;
            }
            Str *s = (Str *)Iter_Get(&v->it);
            curs->idx = v->it.idx;
            curs->ptr = s->bytes;
            curs->end = s->bytes+s->length;
        }
        i64 remaining = ((curs->end) - curs->ptr);
        i64 len = min(remaining, needed);
        if(len == 0){
            break;
        }
        memcpy(((byte *)(&curs->slot))+offset, curs->ptr, len);
        curs->ptr += len;
        curs->total += len;
        offset = curs->total & 7;
        needed = 8 - offset;
    } while(offset != 0);

    curs->type.state |= SUCCESS;
    return curs->type.state;
}

status StrVec_Add(StrVec *v, Str *s){
    status r = Span_Add(v->it.values, (Abstract *)s);
    v->count++;
    v->total += s->length;
    return r;
}

status StrVec_AddBytes(MemCtx *m, StrVec *v, byte *ptr, i64 length){
    Str *s = Span_Get(v->it.values, v->it.values->max_idx);
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
    Iter_Init(&v->it, p);
    return v;
}
