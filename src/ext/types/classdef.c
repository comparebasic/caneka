#include <external.h>
#include <caneka.h>

Lookup *ClassLookup = NULL;

void *ClassDef_Undefined(MemCh *m, FetchTarget *fg, void *_data, void *source){
    Abstract *data = (Abstract *)_data;
    void *args[] = {
        Type_ToStr(m, data->type.of),
        NULL
    };
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "ClassDef not defined for this type $\n", args);
    return NULL;
}

status Class_SetupProp(ClassDef *cls, Str *key){
    status r = READY;
    i32 idx = cls->props->nvalues;
    Single *sg = I32_Wrapped(cls->props->m, cls->props->nvalues);
    r |= Table_Set(cls->props, key, sg);
    Hashed *h = Hashed_Make(cls->propOrder->m, key);
    r |= Span_Set(cls->propOrder, idx, h);
    return r;
}

status Class_SetProp(Object *obj, ClassDef *cls, Str *key, void *value){
    status r = READY;
    Single *sg = (Single *)Table_Get(cls->props, key);
    if(sg == NULL){
        void *args[] = {
            key,
            Type_ToStr(ErrStream->m, obj->objType.of),
            NULL
        };
        Error(Object_GetMem(obj), FUNCNAME, FILENAME, LINENUMBER,
            "Prop @ not found for $", args);
        return ERROR;
    }

    Hashed *h = Span_Get(obj->order, sg->val.i);
    if(h == NULL){
        h = Hashed_Make(obj->order->m, key);
        h->value = value;
        r |= Span_Set(obj->order, sg->val.i, h);
    }else{
        h->value = value;
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

i32 Class_GetPropIdx(ClassDef *cls, Str *key){
    Single *sg = (Single *)Table_Get(cls->props, key);
    if(sg != NULL){
        return sg->val.i;
    }

    return -1;
}

status Class_Register(MemCh *m, ClassDef *cls){
    if(cls->api.toS != NULL){
        Lookup_Add(m, ToStreamLookup, cls->objType.of, (void *)cls->api.toS);
    }

    ClassDef *parent = NULL;
    if(cls->originType.of != ZERO){
        parent = Lookup_Get(ClassLookup, cls->originType.of);
    }else{
        parent = ObjectCls;
    }

    if(parent->props != NULL){
        Table_Underlay(cls->props, parent->props);
    }

    if(cls->api.byKey == ClassDef_Undefined){
        cls->api.byKey = parent->api.byKey;
    }
    if(cls->api.byIdx == ClassDef_Undefined){
        cls->api.byIdx = parent->api.byIdx;
    }
    if(cls->api.getIter == ClassDef_Undefined){
        cls->api.getIter = parent->api.getIter;
    }
    if(cls->api.toS == NULL){
        cls->api.toS = parent->api.toS;
    }

    return Lookup_Add(m, ClassLookup, cls->objType.of, cls);
}

ClassDef *ClassDef_Make(MemCh *m){
    ClassDef *cls = (ClassDef*)MemCh_Alloc(m, sizeof(ClassDef));
    cls->type.of = TYPE_CLASS_DEF;
    cls->props = Table_Make(m);
    cls->atts = Table_Make(m);
    cls->propOrder = Span_Make(m);
    cls->api.byKey = cls->api.byIdx = cls->api.getIter = ClassDef_Undefined;
    return cls;
}

status ClassDef_Init(MemCh *m){
    if(ClassLookup == NULL){
        ClassLookup = Lookup_Make(m, _TYPE_ZERO);
        return SUCCESS;
    }
    return NOOP;
}
