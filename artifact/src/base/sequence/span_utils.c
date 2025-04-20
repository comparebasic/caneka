#include <external.h>
#include <caneka.h>

i32 Span_Capacity(Span *p){
    i32 increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

char **Span_ToCharArr(MemCh *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCh_Alloc(m, sz);
    Iter it;
    Iter_Init(&it, p);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)asIfc(it.value, TYPE_STR);
        if(s != NULL){
            arr[i++] = Str_Cstr(m, s);
        }
    }
    return arr;
}

status Span_SetRaw(Span *p, i32 idx, util *u){
    if(idx < 0 || u == NULL){
        return NOOP;
    }
    p->type.state |= FLAG_SPAN_RAW;
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    it.value = (void *)*u;
    return Iter_Query(&it);
}

util Span_GetRaw(Span *p, i32 idx){
    if(idx < 0){
        return 0;
    }
    if((p->type.state & FLAG_SPAN_RAW) == 0){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "Tried to get raw value from non raw span", NULL);
        return 0;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_GET, idx);
    if(Iter_Query(&it) & SUCCESS){
        return (util)it.value;
    }
    return (util)0;
}


status Span_Cull(Span *p, i32 count){
    Iter it;
    while(count-- > 0){
        i32 idx = p->max_idx;
        if(idx >= 0){
            memset(&it, 0, sizeof(Iter));
            Iter_Setup(&it, p, SPAN_OP_REMOVE, idx);
            Iter_Query(&it);
        }
        p->max_idx--;
    }

    return NOOP;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = -1;
    return SUCCESS;
}


Span *Span_Clone(MemCh *m, Span *p){
    Iter it;
    Iter_Init(&it, p);
    Span *p2 = Span_Make(m);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        if(it.value != NULL){
            Span_Set(p2, it.idx, (Abstract *)it.value); 
        }
    }
    return p2;
}

status Span_Add(Span *p, Abstract *t){
    if(t == NULL){
        return NOOP;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
    it.value = (void *)t;
    status r = Iter_Query(&it);
    return r;
}
