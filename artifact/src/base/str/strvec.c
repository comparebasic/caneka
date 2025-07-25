#include <external.h>
#include <caneka.h>

Str *StrVec_Str(MemCh *m, StrVec *v){
    if(v->total+1 > STR_DEFAULT){
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(m, v->total),
            NULL,
        };
        Error(m, (Abstract *)v, FUNCNAME, FILENAME, LINENUMBER, 
            "Error StrVec is longer than Cstr default of ", args);
        return NULL;
    }
    Str *s = Str_Make(m, v->total+1);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *content = Iter_Get(&it);
        if(TextCharFilter(content->bytes, content->length)){
            Str_Add(s, content->bytes, content->length);
        }
    }
    return s;
}

i64 StrVec_ToFd(StrVec *v, i32 fd){
    Iter it;
    Iter_Init(&it, v->p);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            total += Str_ToFd(s, fd);
        }
    }
    return total;
}

i64 StrVec_FfIter(Iter *it, i64 offset){
    i64 pos = 0;
    while((Iter_Next(it) & END) == 0){
        Str *s = (Str *)it->value;
        if(pos + s->length > offset){
            return offset - pos;
        }
        pos += s->length;
    }
    return offset;
}

StrVec *StrVec_ReAlign(MemCh *m, StrVec *orig){
    StrVec *v = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, orig->p);
    Str *s = Str_Make(m, STR_DEFAULT);
    StrVec_Add(v, s);
    i64 length = 0;
    i64 offset = 0;
    i64 taken = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *os = (Str *)it.value;
        offset = 0;
        length = os->length;
        taken = Str_Add(s, os->bytes+offset, length);
        offset += taken;
        length -= taken;
        v->total += taken;
        while(length > 0){
            s = Str_Make(m, STR_DEFAULT);
            StrVec_Add(v, s);
            taken = Str_Add(s, os->bytes+offset, length);
            offset += taken;
            length -= taken;
            v->total += taken;
        }
    }

    return v;
}

status StrVec_NextSlot(StrVec *v, Cursor *curs){
    i64 offset = 0;
    util needed = 8;
    curs->slot = 0;
    do {
        if(curs->ptr == NULL){
            if((Iter_Next(&curs->it) & END) != 0){
                return ERROR;
            }
            Str *s = (Str *)curs->it.value;
            curs->ptr = s->bytes;
            curs->end = s->bytes+(s->length-1);
        }
        i64 remaining = ((curs->end+1) - curs->ptr);
        i64 len = min(remaining, needed);
        if(len == 0){
            break;
        }
        memcpy(((byte *)(&curs->slot))+offset, curs->ptr, len);
        if(len == remaining){
            curs->ptr = NULL;
        }else{
            curs->ptr += len;
        }
        curs->offset += len;
        offset = curs->offset & 7;
        needed = 8 - offset;
    } while(offset != 0);

    curs->type.state |= SUCCESS;
    return curs->type.state;
}

i32 StrVec_GetIdx(StrVec *v, Str *s){
    Iter it;
    Iter_Init(&it, v->p);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *_s = (Str *)it.value;
        if(_s != NULL && _s == s){
            return it.idx;
        }
    }
    return -1;
}

Abstract *StrVec_Clone(MemCh *m, Abstract *a){
    StrVec *v = (StrVec *)as(a, TYPE_STRVEC);
    StrVec *new = StrVec_Make(m);
    new->total = v->total;
    new->p = Span_Clone(m, v->p);
    return (Abstract *)new;
}

status StrVec_Add(StrVec *v, Str *s){
    status r = Span_Add(v->p, (Abstract *)s);
    v->total += s->length;
    return r;
}

status StrVec_AddVec(StrVec *v, StrVec *v2){
    Iter it;
    Iter_Init(&it, v2->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            StrVec_Add(v, s);
        }
    }
    return SUCCESS;
}

status StrVec_AddBytes(MemCh *m, StrVec *v, byte *ptr, i64 length){
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
            v->total += (word)take;
        }
        s = NULL;
    }
    return r;
}

StrVec *StrVec_From(MemCh *m, Str *s){
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, s);
    return v;
}

StrVec *StrVec_Make(MemCh *m){
    StrVec *v = MemCh_Alloc(m, sizeof(StrVec));
    v->type.of = TYPE_STRVEC;
    v->p = Span_Make(m);
    return v;
}
