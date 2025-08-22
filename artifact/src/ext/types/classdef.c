#include <external.h>
#include <caneka.h>

Lookup *ClassLookup = NULL;

Abstract *ClassDef_Undefined(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    Abstract *args[] = {
        (Abstract *)Type_ToStr(m, data->type.of),
        NULL
    };
    Error(m, (Abstract *)source, FUNCNAME, FILENAME, LINENUMBER, 
        "ClassDef not defined for this type $\n", args);
    return NULL;
}

status Class_SetupProp(ClassDef *cls, Str *key){
    Single *sg = I32_Wrapped(cls->props->m, cls->props->nvalues);
    return Table_Set(cls->props, (Abstract *)key, (Abstract *)sg);
}

status Class_SetProp(Abstract *inst, ClassDef *cls, Str *key, Abstract *value){
    Single *sg = Table_Get(cls, (Abstract *)key);
    if(sg != NULL && inst->type.of == TYPE_ORDTABLE){
        Hashed *h = Span_Get(inst->order, sg->val.i);
        if(h = NULL){
            h = Hashed_Make(m);
            h->key = key;
            h->value = value;
            Span_Set(inst->order, sg->val.i, (Abstract *)h);
        }else{
            h->value = value;
        }
    }
    return NULL;
}

Iter *Class_GetIter(Object *o){
    if(inst->type.of == TYPE_PATHTABLE){
        Iter *it = Iter_Make(m, o->order);
        Iter_GoToIdx(it, o->cls->props->nvalues);
        return (Abstract *)it;
    }
    return NULL;
}

status Class_GetProp(ClassDef *cls, Str *key){
    Single *sg = Table_Get(cls, (Abstract *)key);
    if(sg != NULL && inst->type.of == TYPE_ORDTABLE){
        Hashed *h = Span_Get(inst->order, sg->val.i);
        if(h = NULL){
            return h->value;
        }
    }
    return NULL;
}

status Class_Register(MemCh *m, ClassDef *cls){
    if(cls->api.toS != NULL){
        Lookup_Add(m, ToStreamLookup, cls->objType.of, (void *)cls->toS);
    }
    if((cls->type.state & CLASS_STRUCT) == 0){
        if(cls->originType.of != ZERO){
            ClassDef *parent = Lookup_Get(ClassLookup, cls->originType.of);
            if(parent != NULL){
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
                if(cls->api.toS == ClassDef_Undefined){
                    cls->api.toS = parent->api.toS;
                }
            }
        }
    }

    return Lookup_Add(m, ClassLookup, cls->type.inst, cls);
}

ClassDef *ClassDef_Make(MemCh *m){
    ClassDef *cls = (ClassDef*)MemCh_Alloc(m, sizeof(ClassDef));
    cls->type.of = TYPE_CLASS_DEF;
    cls->props = Table_Make(m);
    cls->atts = Table_Make(m);
    cls->api.byKey = cls->api.byIdx = cls->api.getIter = cls->api.toS = ClassDef_Undefined;
    return cls;
}

status ClassDef_Init(MemCh *m){
    if(ClassLookup == NULL){
        ClassLookup = Lookup_Make(m, _TYPE_ZERO);
        return SUCCESS;
    }
    return NOOP;
}
