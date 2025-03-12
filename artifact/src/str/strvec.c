#include <external.h>
#include <caneka.h>

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
