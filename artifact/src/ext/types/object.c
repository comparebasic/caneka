#include <external.h>
#include <caneka.h>

boolean Object_TypeMatch(Abstract *a, cls typeOf){
    return (a->type.of == TYPE_OBJECT && 
        ((Object *)a)->objType.of == typeOf) || (a->type.of == typeOf);
}

Object *Object_As(Object *obj, cls typeOf){
    boolean pass = (obj->type.of == TYPE_OBJECT && 
        ((Object *)obj)->objType.of == typeOf);

    if(!pass){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, typeOf),
            (Abstract *)Type_ToStr(ErrStream->m, obj->type.of),
            (Abstract *)Type_ToStr(ErrStream->m, obj->objType.of),
            NULL
        };
        Error(ErrStream->m, (Abstract *)obj, FUNCNAME, FILENAME, LINENUMBER,
            "Error object is not of expected type $, have $/$", args);
    }
    return obj;
}

Abstract *Object_GetIter(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    Object *obj = (Object *)as(data, TYPE_OBJECT);
    Iter *it = Iter_Make(m, obj->order);
    Iter_GoToIdx(it, obj->propMask);
    return (Abstract *)it;
}

Abstract *Object_GetProp(Object *obj, Str *key){
    ClassDef *cls = Lookup_Get(ClassLookup, obj->objType.of);
    i32 idx = Class_GetPropIdx(cls, key);
    if(idx == -1){
        Abstract *args[] = {
            (Abstract *)key,
            (Abstract *)Type_ToStr(obj->order->m, obj->objType.of),
            NULL
        };
        Error(obj->order->m, (Abstract *)obj, FUNCNAME, FILENAME, LINENUMBER,
            "Prop @ not found on object of $\n", args);
        return NULL;
    }

    Hashed *h = Span_Get(obj->order, idx);
    if(h != NULL){
        return h->value;
    }

    return NULL;
}

Abstract *Object_Get(Object *obj, Abstract *key){
    return Table_Get(obj->tbl, key);
}

Object *Object_GetOrMake(Object *pt, Abstract *key, word op){
    Abstract *a = Object_Get(pt, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Object *new = Object_Make(pt->tbl->m, pt->objType.of);
        Object_Set(pt, key, (Abstract *)new);
        return new;
    }else if(a->type.of == TYPE_OBJECT){
        return (Object *)a;
    }else{
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, a->type.of),
            (Abstract *)key,
            NULL
        };
        Error(ErrStream->m, (Abstract *)pt, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty Object where a $ value already exists for key @",
            args);
        return NULL;
    }
}

status Object_Depth(Abstract *a){
    if(a->type.of == TYPE_OBJECT){
        return ((Object *)a)->depth;
    }
    return 1;
}

Object *Object_ByPath(Object *obj, StrVec *path, Abstract *value, word op){
    DebugStack_Push(obj, obj->type.state);
    if(obj->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)path,
            (Abstract *)obj,
            NULL
        };
        Out("^c.Adding & to @^0.\n", args);
    }
    if((path->type.state & STRVEC_PATH) == 0){
       IoUtil_Annotate(Object_GetMem(obj), path); 
    }
    Iter keysIt;
    Iter_Init(&keysIt, path->p);
    Str *key = NULL;
    Object *current = obj;
    i16 depth = 1;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Object_GetOrMake(current, (Abstract *)key, op);
            if(current == NULL){
                return NULL;
            }
            key = NULL;
        }else{
            key = item;
            if((keysIt.type.state & LAST) && (op & SPAN_OP_SET)){
                Object_Set(current, (Abstract *)key, value);
                if(depth > obj->depth){
                    obj->depth = depth;
                }
            }
        }
        depth++;
    }

    if(key != NULL && (key->type.state & (LAST|MORE)) == 0){
        current = Object_GetOrMake(current, (Abstract *)key, op);
        if(current == NULL){
            return NULL;
        }
    }

    DebugStack_Pop();
    return current;
}

Hashed *Object_Set(Object *obj, Abstract *key, Abstract *value){
    obj->type.state &= ~OUTCOME_FLAGS;

    Hashed *h = Table_SetHashed(obj->tbl, key, value);
    obj->type.state |= obj->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(obj->order, (Abstract *)h);
        if(obj->order->type.state & SUCCESS){
            h->orderIdx = obj->order->max_idx;
        }
        obj->type.state |= obj->order->type.state & OUTCOME_FLAGS; 
    }

    if(obj->type.state & DEBUG){
        ClassDef *cls = Lookup_Get(ClassLookup, obj->objType.of);
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, obj->objType.of),
            (Abstract *)key,
            (Abstract *)I32_Wrapped(OutStream->m, 
                obj->order->nvalues -cls->propOrder->nvalues),
            NULL,
        };
        Out("^p.Object_Set($ [$]/$)^0\n", args);
    }
    return h;
}

Hashed *Object_SetProp(Object *obj, Str *key, Abstract *value){
    ClassDef *cls = Lookup_Get(ClassLookup, obj->objType.of);
    if(cls != NULL){
        Single *sg = (Single *)Table_Get(cls->props, (Abstract *)key);
        if(sg != NULL){
            Hashed *h = Span_Get(obj->order, sg->val.i);
            h->value = value;
            return h;
        }
    }

    Abstract *args[] = {
        (Abstract *)key,
        (Abstract *)Type_ToStr(ErrStream->m, obj->objType.of),
        NULL
    };
    Error(Object_GetMem(obj), (Abstract *)obj, FUNCNAME, FILENAME, LINENUMBER,
        "Unable to find prop @ on ClassDef $", args);
    return NULL;
}

Hashed *Object_SetPropByIdx(Object *obj, i32 idx, Abstract *value){
    Hashed *h = Span_Get(obj->order, idx);
    h->value = value; 
    return h;
}

Abstract *Object_GetPropByIdx(Object *obj, i32 idx){
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
    Object *obj = (Object *)MemCh_Alloc(m, sizeof(Object));
    obj->type.of = TYPE_OBJECT;
    obj->tbl = Table_Make(m);

    if(typeOf != ZERO){
        ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
        if(cls == NULL){
            Abstract *args[] = {
                (Abstract *)Type_ToStr(m, typeOf),
                NULL
            };
            Error(m, NULL, FUNCNAME, FILENAME, LINENUMBER,
                "ClassDeff $ not found", args);
            return NULL;

        }
        obj->order = Span_Clone(m, cls->propOrder); 
        obj->propMask = obj->order->nvalues;
        obj->objType.of = typeOf;
    }else{
        obj->order = Span_Make(m);
        obj->objType.of = TYPE_OBJECT;
    }

    return obj;
}
