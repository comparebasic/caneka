#include <external.h>
#include <caneka.h>

Str *StrVec_ToStr(MemCh *m, StrVec *v, word length){
    Abstract *args[2];
    if(v->total+1 > STR_MAX || v->total+1 > length){
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(m, v->total),
            (Abstract *)I16_Wrapped(m, length),
            NULL,
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error StrVec is longer than Cstr default of $ length of $ ", args);
        return NULL;
    }
    Str *s = Str_Make(m, length);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *content = Iter_Get(&it);
        if(TextCharFilter(content->bytes, content->length)){
            Str_Add(s, content->bytes, content->length);
        }else{
            args[0] = (Abstract *)content;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Failed text char filter &", args);
            return NULL;
        }
    }
    return s;
}

Str *StrVec_Str(MemCh *m, StrVec *v){
    return StrVec_ToStr(m, v, v->total+1);
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
    if(orig->type.state & STRVEC_ALIGNED){
        return orig;
    }
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

    v->type.state |= STRVEC_ALIGNED;
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

StrVec *StrVec_Copy(MemCh *m, StrVec *_v){
    StrVec *v = (StrVec *)as((Abstract *)_v, TYPE_STRVEC);
    StrVec *new = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Str_Clone(m, (Str *)Iter_Get(&it));
        StrVec_Add(new, s);
    }
    return new;
}

Abstract *StrVec_Clone(MemCh *m, Abstract *a){
    StrVec *v = (StrVec *)as(a, TYPE_STRVEC);
    StrVec *new = StrVec_Make(m);
    new->total = v->total;
    new->p = Span_Clone(m, v->p);
    return (Abstract *)new;
}

status StrVec_Add(StrVec *v, Str *s){
    s = (Str *)as((Abstract *)s, TYPE_STR);
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

StrVec *StrVec_FromLongBytes(MemCh *m, byte *bytes, i32 length){
    StrVec *v = StrVec_Make(m);
    while(length > STR_MAX){
        Str *s = Str_Ref(m, bytes, STR_MAX, STR_MAX, ZERO);
        StrVec_Add(v, s);
        length -= s->length;
        bytes += s->length;
    }

    Str *s = Str_Ref(m, bytes, length, length, ZERO);
    StrVec_Add(v, s);
    return v;
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
