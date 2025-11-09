#include <external.h>
#include <caneka.h>

Span *Span_CopyRange(MemCh *m, Span *p, Coord *cr){
    Span *ret = Span_Make(m);
    Iter it;
    Iter_Init(&it, p);
    Iter_GetByIdx(&it, (i32)cr->a);
    while(it.idx < cr->b){
        void *a = Iter_Get(&it);
        Span_Add(ret, a);
        if(Iter_Next(&it) & END){
            break;
        }
    }
    return ret;
}

i32 Span_Capacity(Span *p){
    i32 increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

i32 Span_Has(Span *p, void *a){
   Iter it;  
   Iter_Init(&it, p);
   while((Iter_Next(&it) & END) == 0){
        if(Equals(Iter_Get(&it), a)){
            return it.idx;
        }
   }
   return -1;
}

boolean Span_Equals(Span *a, Span *b){
    Iter itA;
    Iter_Init(&itA, a);
    Iter itB;
    Iter_Init(&itB, b);
    while((Iter_Next(&itA) & END) == 0 && (Iter_Next(&itB) & END) == 0){
        if(!Equals(Iter_Get(&itA), Iter_Get(&itB))){
            return FALSE;
        }
    }
    if((itB.type.state & END) != (itB.type.state & END)){
        return FALSE;
    }
    return TRUE;
}

char **Span_ToCharArr(MemCh *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = (char **)Bytes_Alloc(m, sz, TYPE_POINTER_ARRAY);

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
    Iter_Init(&it, p);
    return Iter_SetByIdx(&it, idx, (void *)*u);
}

util Span_GetRaw(Span *p, i32 idx){
    if(idx < 0){
        return 0;
    }
    if((p->type.state & FLAG_SPAN_RAW) == 0){
        Error(p->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Tried to get raw value from non raw span", NULL);
        return 0;
    }
    Iter it;
    Iter_Init(&it, p);
    return (util)Iter_GetByIdx(&it, idx);
}

status Span_Wipe(Span *p){
    return Span_Cull(p, p->nvalues);
}

status Span_Cull(Span *p, i32 count){
    status r = READY;
    Iter it;
    Iter_Init(&it, p);
    while(count-- > 0){
        i32 idx = p->max_idx;
        if(idx >= 0){
            r |= Iter_Prev(&it);
            Iter_RemoveByIdx(&it, it.idx);
        }
    }

    return r;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = -1;
    return SUCCESS;
}

Span *Span_CloneShallow(MemCh *m, Span *p){
    Iter it;
    Iter_Init(&it, p);
    Span *p2 = Span_Make(m);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        Span_Set(p2, it.idx, Iter_Get(&it)); 
    }
    return p2;
}

Span *Span_Clone(MemCh *m, Span *p){
    Iter it;
    Iter_Init(&it, p);
    Span *p2 = Span_Make(m);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        void *value = Iter_Get(&it);
        if(value != NULL){
            Abstract *item = Clone(m, value);
            Span_Set(p2, it.idx, item); 
        }
    }
    return p2;
}

status Span_Insert(Span *p, i32 idx, void *t){
    if(t == NULL){
        return NOOP;
    }
    Iter it;
    Iter_Init(&it, p);
    return Iter_Insert(&it, idx, t);
}

status Span_AddRaw(Span *p, util *u){
    if((p->type.state & FLAG_SPAN_RAW) == 0){
        Abstract *args[2];
        args[0] = (Abstract *)Str_FuncName(p->m);
        args[1] = NULL;
        Error(p->m, FUNCNAME, FILENAME, LINENUMBER,
            "$ requires RAW flag for span", args);
        return ERROR;
    }
    p->type.state &= ~(ERROR|SUCCESS|NOOP);
    if(u == NULL){
        p->type.state |= NOOP;
        return p->type.state;
    }
    Iter it;
    Iter_Init(&it, p);
    p->type.state |= Iter_Add(&it, (void *)*u) & (ERROR|SUCCESS|NOOP);
    return p->type.state;
}

status Span_Add(Span *p, void *t){
    p->type.state &= ~(ERROR|SUCCESS|NOOP);
    if(t == NULL){
        p->type.state |= NOOP;
        return p->type.state;
    }
    Iter it;
    Iter_Init(&it, p);
    p->type.state |= Iter_Add(&it, t) & (ERROR|SUCCESS|NOOP);
    return p->type.state;
}
