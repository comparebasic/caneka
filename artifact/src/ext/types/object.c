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

Object *Object_GetOrMake(Object *pt, Abstract *key){
    Abstract *a = Object_Get(pt, key);
    if(a == NULL){
        Object *new = Object_Make(pt->tbl->m, ZERO);
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

status Object_AddByPath(Object *pt, StrVec *path, Abstract *value){
    DebugStack_Push(pt, pt->type.state);
    if(pt->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)path,
            (Abstract *)pt,
            NULL
        };
        Out("^c.Adding @ to @^0.\n", args);
    }
    status r = READY;
    if((path->type.state & STRVEC_PATH) == 0){
       IoUtil_Annotate(Object_GetMem(pt), path); 
    }
    Iter keysIt;
    Iter_Init(&keysIt, path->p);
    Str *key = NULL;
    Object *current = pt;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Object_GetOrMake(current, (Abstract *)key);
            key = NULL;
            r |= PROCESSING;
        }else{
            key = item;
            if(keysIt.type.state & LAST){
                Object_Set(current, (Abstract *)key, value);
                r |= SUCCESS;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

Hashed *Object_Set(Object *pt, Abstract *key, Abstract *value){
    pt->type.state &= ~OUTCOME_FLAGS;

    Hashed *h = Table_SetHashed(pt->tbl, key, value);
    pt->type.state |= pt->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(pt->order, (Abstract *)h);
        if(pt->order->type.state & SUCCESS){
            h->orderIdx = pt->order->max_idx;
        }
        pt->type.state |= pt->order->type.state & OUTCOME_FLAGS; 
    }
    return h;
}

Hashed *Object_GetPropByIdx(Object *obj, i32 idx){
    return Span_Get(obj->order, idx);
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
        obj->order = Span_Clone(m, cls->propOrder); 
        obj->propMask = obj->order->nvalues;
        obj->objType.of = typeOf;
    }else{
        obj->order = Span_Make(m);
    }

    return obj;
}
