#include <external.h>
#include <caneka.h>

boolean Object_TypeMatch(void *_a, cls typeOf){
    Abstract *a = (Abstract *)_a;
    return (a->type.of & TYPE_OBJECT && a->type.of == typeOf);
}

Object *Object_As(Object *obj, cls typeOf){
    boolean pass = (obj->type.of == typeOf);

    if(!pass){
        void *args[] = {
            Type_ToStr(ErrStream->m, typeOf),
            Type_ToStr(ErrStream->m, obj->type.of),
            NULL
        };
        Error(Object_GetMem(obj), FUNCNAME, FILENAME, LINENUMBER,
            "Error object is not of expected type $, have $", args);
    }
    return obj;
}

status Object_Lay(Object *dest, Object *src, cls typeOf, boolean overlay){
    status r = READY;
    if(src == NULL){
        return r;
    }
    ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
    Iter it;
    if(cls != NULL){
        Iter_Init(&it, src->order);
        while((Iter_Next(&it) & END) == 0){
            if(it.idx > cls->props->max_idx){
                r |= Span_Add(dest->order, Iter_Get(&it));
            }else if(overlay){
                r |= Span_Set(dest->order, it.idx, Iter_Get(&it));
            }
        }
    }

    Iter_Init(&it, src->tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed*)Iter_Get(&it);
        if(h != NULL){
            if(overlay || (Table_Get(dest->tbl, h->key) == NULL)){
                r |= Table_Set(dest->tbl, h->key, h->value);
            }
        }
    }
    return r;
}

status Object_Merge(Object *dest, Object *src, cls typeOf){
    return Object_Lay(dest, src, typeOf, TRUE);
}

status Object_SetKey(Iter *it, Object *obj, void *key){
    if(it->p == NULL){
        Iter_Init(it, obj->tbl);
    }
    return Table_SetKey(it, key);
}

Hashed *Object_SetValue(Iter *it, Object *obj, void *a){
    Hashed *h = Table_SetValue(it, a);
    if(h != NULL){
        Span_Add(obj->order, h);
        return h;
    }

    return NULL;
}

Object *Object_Filter(Object *obj, SourceFunc func, void *source){
    MemCh *m = Object_GetMem(obj);
    /*
    Iter it;
    Iter_Init(&it, obj->tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL && h->value != NULL && h->value->type.of == TYPE_OBJECT){
            void *args[] = {
                h->key, NULL
            };
            Out("^y.Filtering @^0\n", args);
            if(func(m, h->value, source)){
                Object_Filter((Object *)h->value, func, source);
            }
        }
    }
    */

    return obj;
}

void *Object_GetIter(MemCh *m, FetchTarget *fg, void *data, void *source){
    if((((Abstract *)data)->type.of & TYPE_OBJECT) == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Expected object to get iter from", NULL);
        return NULL;
    }
    Object *obj = (Object *)data;
    Iter *it = Iter_Make(m, obj->order);
    Iter_GoToIdx(it, obj->propMask);
    return it;
}

void *Object_GetProp(Object *obj, Str *key){
    ClassDef *cls = Lookup_Get(ClassLookup, obj->type.of);
    i32 idx = Class_GetPropIdx(cls, key);
    if(idx == -1){
        void *args[] = {
            key,
            Type_ToStr(obj->order->m, obj->type.of),
            NULL
        };
        Error(obj->order->m, FUNCNAME, FILENAME, LINENUMBER,
            "Prop @ not found on object of $\n", args);
        return NULL;
    }

    Hashed *h = Span_Get(obj->order, idx);
    if(h != NULL){
        return h->value;
    }

    return NULL;
}

void *Object_Get(Object *obj, void *key){
    return Table_Get(obj->tbl, key);
}

Hashed *Object_GetHashed(Object *obj, void *key){
    return Table_GetHashed(obj->tbl, key);
}

Object *Object_GetOrMake(Object *pt, void *key, word op){
    Abstract *a = (Abstract *)Object_Get(pt, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Object *new = Object_Make(pt->tbl->m, pt->type.of);
        Object_Set(pt, key, new);
        return new;
    }else if(a->type.of & TYPE_OBJECT){
        return (Object *)a;
    }else{
        void *args[] = {
            Type_ToStr(ErrStream->m, a->type.of),
            key,
            a,
            NULL
        };
        Error(Object_GetMem(pt), FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty Object where a $ value already exists for key @, or is not an object @",
            args);
        return NULL;
    }
}

status Object_Depth(void *_a){
    Abstract *a = (Abstract *)_a;
    if(a->type.of & TYPE_OBJECT){
        return ((Object *)a)->depth;
    }
    return 1;
}

Object *Object_ByPath(Object *obj, StrVec *path, void *value, word op){
    DebugStack_Push(obj, obj->type.state);
    if((path->type.state & STRVEC_PATH) == 0){
       IoUtil_Annotate(Object_GetMem(obj), path); 
    }
    Iter keysIt;
    Iter_Init(&keysIt, path->p);

    if(obj->type.state & DEBUG){
        void *args[4];
        args[0] = NULL;
        args[1] = path->p;
        args[2] = obj->tbl;
        args[3] = NULL;
        Out("^c.^{STACK.name} keys->p:@ from @^0\n", args);
    }

    Str *key = NULL;
    Object *current = obj;
    i16 depth = 1;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Object_GetOrMake(current, key, op);
            if(current == NULL){
                DebugStack_Pop();
                return NULL;
            }
            key = NULL;
        }else{
            key = item;
            if((keysIt.type.state & LAST) && (op & SPAN_OP_SET)){
                Object_Set(current, key, value);
                if(depth > obj->depth){
                    obj->depth = depth;
                }
            }
        }
        depth++;
    }

    if(key != NULL && (key->type.state & (LAST|MORE)) == 0){
        current = Object_GetOrMake(current, key, op);
        if(current == NULL){
            DebugStack_Pop();
            return NULL;
        }
    }

    DebugStack_Pop();
    return current;
}

i32 Object_Add(Object *obj, void *value){
    Hashed *h = Hashed_Make(Object_GetMem(obj), NULL);
    h->value = value;
    Span_Add(obj->order, h);
    return obj->order->max_idx;
}

Hashed *Object_Set(Object *obj, void *key, void *value){
    obj->type.state &= ~OUTCOME_FLAGS;

    if(obj->type.state & DEBUG){
        void *args[] = {
            key,
            value,
            NULL
        };
        Out("^b.>>> Setting: @ -> @^0\n", args);
    }

    Hashed *h = Table_SetHashed(obj->tbl, key, value);

    obj->type.state |= obj->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(obj->order, h);
        if(obj->order->type.state & SUCCESS){
            h->orderIdx = obj->order->max_idx;
        }
        obj->type.state |= obj->order->type.state & OUTCOME_FLAGS; 
    }

    if(obj->type.state & DEBUG){
        ClassDef *cls = Lookup_Get(ClassLookup, obj->type.of);
        void *args[] = {
            Type_ToStr(OutStream->m, obj->type.of),
            key,
            I32_Wrapped(OutStream->m, 
                obj->order->nvalues - cls->propOrder->nvalues),
            NULL,
        };
        Out("^p.Object_Set($ [$]/$)^0\n", args);
    }
    return h;
}

Hashed *Object_SetProp(Object *obj, Str *key, void *value){
    ClassDef *cls = Lookup_Get(ClassLookup, obj->type.of);
    if(cls != NULL){
        Single *sg = (Single *)Table_Get(cls->props, key);
        if(sg != NULL){
            Hashed *h = Span_Get(obj->order, sg->val.i);
            h->value = value;
            return h;
        }
    }

    void *args[] = {
        key,
        Type_ToStr(ErrStream->m, obj->type.of),
        NULL
    };
    Error(Object_GetMem(obj), FUNCNAME, FILENAME, LINENUMBER,
        "Unable to find prop @ on ClassDef $", args);
    return NULL;
}

Hashed *Object_SetPropByIdx(Object *obj, i32 idx, void *value){
    Hashed *h = Span_Get(obj->order, idx);
    h->value = value; 
    return h;
}

void *Object_GetPropByIdx(Object *obj, i32 idx){
    Hashed *h = Object_GetByIdx(obj, idx);
    return h->value;
}

Hashed *Object_GetByIdx(Object *tbl, i32 idx){
    return (Hashed *)Span_Get(tbl->order, idx);
}

boolean Object_IsBlank(Object *pt){
    return pt->tbl->nvalues == 0;
}

Object *Object_Make(MemCh *m, cls typeOf){
    if(typeOf == ZERO){
        typeOf = TYPE_OBJECT;
    }else if((typeOf & TYPE_OBJECT) == 0){
        void *args[] = {Type_ToStr(m, typeOf), NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Object_Make can only be made from a typeOf that contains TYPE_OBJECT, have $",
        args);
        return NULL;
    }
    Object *obj = (Object *)MemCh_Alloc(m, sizeof(Object));
    obj->type.of = typeOf;
    obj->tbl = Table_Make(m);

    if(typeOf != TYPE_OBJECT){
        ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
        if(cls == NULL){
            void *args[] = {
                Type_ToStr(m, typeOf),
                NULL
            };
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "ClassDeff $ not found", args);
            return NULL;

        }
        obj->order = Span_Clone(m, cls->propOrder); 
        obj->propMask = obj->order->nvalues;
    }else{
        obj->order = Span_Make(m);
    }

    return obj;
}
