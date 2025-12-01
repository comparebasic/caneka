#include <external.h>
#include "base_module.h"

status StrVec_AddChain(StrVec *v, void *args[]){
    status r = READY;
    Abstract **ptr = (Abstract **)args;
    MemCh *m = v->p->m;
    while(*ptr != NULL){
        Abstract *a = (Abstract *)*ptr;
        if(a->type.of == TYPE_STRVEC){
            StrVec_AddVec(v, (StrVec *)a);
        }else if(a->type.of == TYPE_STR){
            Str *s = (Str *)a;
            if(s->type.state & STRING_COPY && v->p->nvalues > 0){
                Str *last = Span_Get(v->p, v->p->max_idx);
                if(last->length + s->length < last->alloc){
                    Str_Add(last, s->bytes, s->length);
                    r |= SUCCESS;
                }else{
                    Str *new = Str_Make(m, last->length+s->length+1);
                    Str_Add(new, last->bytes, last->length);
                    Str_Add(new, s->bytes, s->length);
                    Span_Set(v->p, v->p->max_idx, new);
                    v->total += s->length;
                    r |= SUCCESS;
                }
            }else{
                StrVec_Add(v, (Str *)a);
                r |= SUCCESS;
            }
        }
        ptr++;
    }
    
    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status StrVec_Decr(StrVec *v, i64 amount){
    if(amount > v->total){
        return ERROR;
    }
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->length > amount){
            s->length -= amount;
            amount = 0;
            v->total -= amount;
        }else if(amount > 0){
            Iter_Remove(&it);
            v->total -= s->length;
            amount -= s->length;
        }else{
            break;
        }
    }
    return SUCCESS;
}

status Str_AddVec(Str *s, StrVec *v){
    status r = READY;
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *_s = Iter_Get(&it);
        Str_Add(s, _s->bytes, _s->length);
        r |= (SUCCESS |(s->type.state & ERROR));
    }
    return r;
}

status StrVec_Incr(StrVec *v, i64 amount){
    Span *p = Span_Make(v->p->m);
    if(amount > v->total){
        return ERROR;
    }
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->length > amount){
            Str_Incr(s, amount);
            Span_Add(p, s);
            amount = 0;
        }else if(amount == 0){
            Span_Add(p, s);
        }else{
            amount -= s->length;
        }
    }
    v->p = p;
    v->total = v->total - amount;
    return SUCCESS;
}

Str *StrVec_ToStr(MemCh *m, StrVec *v, word length){
    void *args[2];
    if(v->total+1 > STR_MAX || v->total+1 > length){
        void *args[] = {
            I64_Wrapped(m, v->total),
            I16_Wrapped(m, length),
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
            args[0] = content;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Failed text char filter &", args);
            return NULL;
        }
    }
    return s;
}

Str *StrVec_Str(MemCh *m, StrVec *v){
    if(v == NULL){
        return NULL;
    }
    return StrVec_ToStr(m, v, v->total+1);
}

Str *StrVec_StrTo(MemCh *m, StrVec *v, i32 anchor){
    i64 total = v->total;
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Prev(&it) & END) == 0 && it.idx >= anchor){
        Str *s = Iter_Get(&it); 
        total -= s->length;
    }
    Str *s = Str_Make(m, total+1);
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0 && it.idx <= anchor){
        Str *_s = Iter_Get(&it); 
        Str_Add(s, _s->bytes, _s->length);
    }
    return s;
}


Str *StrVec_StrCombo(MemCh *m, void *_a, void *_b){
    Abstract *a = (Abstract *)_a;
    Abstract *b = (Abstract *)_b;
    word tailLength = 0;
    StrVec *v = NULL;
    if(a->type.of == TYPE_STR){
        v = StrVec_From(m, (Str *)a);
    }else if(a->type.of == TYPE_STRVEC){
        v = (StrVec *)a;
    }else{
        void *args[] = {a, b, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error combo requires Str or StrVec arguments only: @, @", args);
        return NULL;
    }

    if(b->type.of == TYPE_STR){
        tailLength = ((Str *)b)->length;
    }else if(b->type.of == TYPE_STRVEC){
        tailLength =  ((StrVec *)b)->total;
    }else{
        void *args[] = {a, b, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error combo requires Str or StrVec arguments only: @, @", args);
        return NULL;
    }

    Str *s = Str_Make(m, v->total + tailLength + 1);
    if(s == NULL){
        return s;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((s->type.state & ERROR) == 0 && (Iter_Next(&it) & END) == 0){
        Str *_s = (Str *)Iter_Get(&it);
        Str_Add(s, _s->bytes, _s->length);
    }

    if(b->type.of == TYPE_STR){
        Str_Add(s, ((Str *)b)->bytes, ((Str *)b)->length);
    }else if(b->type.of == TYPE_STRVEC){
        Iter_Init(&it, ((StrVec *)b)->p);
        while((s->type.state & ERROR) == 0 && (Iter_Next(&it) & END) == 0){
            Str *_s = (Str *)Iter_Get(&it);
            Str_Add(s, _s->bytes, _s->length);
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
    StrVec *v = (StrVec *)as(_v, TYPE_STRVEC);
    StrVec *new = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Str_Clone(m, (Str *)Iter_Get(&it));
        StrVec_Add(new, s);
    }
    return new;
}

void *StrVec_Clone(MemCh *m, void *a){
    StrVec *v = (StrVec *)as(a, TYPE_STRVEC);
    StrVec *new = StrVec_Make(m);
    new->total = v->total;
    new->p = Span_Clone(m, v->p);
    return new;
}

i32 StrVec_Add(StrVec *v, Str *s){
    i32 anchor = v->p->max_idx;
    s = (Str *)as(s, TYPE_STR);
    status r = Span_Add(v->p, s);
    v->total += s->length;
    return anchor;
}

i32 StrVec_AddVec(StrVec *v, StrVec *v2){
    i32 idx = v->p->max_idx;
    Iter it;
    Iter_Init(&it, v2->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            StrVec_Add(v, s);
        }
    }
    return idx;
}

i32 StrVec_AddVecAfter(StrVec *v, StrVec *v2, i32 idx){
    i32 start = v->p->max_idx;
    Iter it;
    Iter_Init(&it, v2->p);
    while((Iter_Next(&it) & END) == 0){
        if(it.idx >= idx){
            Str *s = (Str *)it.value;
            if(s != NULL){
                StrVec_Add(v, s);
            }
        }
    }
    return start;
}

status StrVec_Pop(StrVec *v){
    if(v->total == 0){
        return NOOP;
    }
    Iter it;
    Iter_Init(&it, v->p);
    Str *s =  Iter_GetByIdx(&it, it.p->max_idx);
    Iter_Remove(&it);
    v->total -= s->length;
    return SUCCESS;
}

status StrVec_PopTo(StrVec *v, i32 idx){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Prev(&it) & END) == 0){
        Str *s = Iter_Get(&it); 
        if(it.idx <= idx){
            break;
        }
        v->total -= s->length;
        Iter_Remove(&it);
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

StrVec *StrVec_StrVec(MemCh *m, void *_a){
    Abstract * a = (Abstract *)_a;
    if(a->type.of == TYPE_STRVEC){
        return (StrVec *)a;
    }else if(a->type.of == TYPE_STR){
        return StrVec_From(m, (Str *)a);
    }else{
        Error(m, FUNCNAME, FILENAME, LINENUMBER,    
            "StrVec_From only possible from Str or StrVec", NULL);
        return NULL;
    }
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
