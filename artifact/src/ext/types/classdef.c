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

status Class_Register(MemCh *m, ClassDef *cls){
    if(cls->toS != NULL){
        Lookup_Add(m, ToStreamLookup, cls->objType.of, (void *)cls->toS);
    }
    if(cls->parentType.of != ZERO){
        ClassDef *parent = Lookup_Get(ClassLookup, cls->parentType.of);
        if(parent != NULL){
            if(parent->atts != NULL){
                Table_Underlay(cls->atts, parent->atts);
            }
            if(parent->methods != NULL){
                Table_Underlay(cls->methods, parent->methods);
            }
            if(cls->byKey == NULL){
                cls->byKey = parent->byKey;
            }
            if(cls->byIdx == NULL){
                cls->byIdx = parent->byIdx;
            }
            if(cls->getIter == NULL){
                cls->getIter = parent->getIter;
            }
            if(cls->toS == NULL){
                cls->toS = parent->toS;
            }
        }
    }

    return Lookup_Add(m, ClassLookup, cls->objType.of, cls);
}

ClassDef *ClassDef_Make(MemCh *m){
    ClassDef *cls = (ClassDef*)MemCh_Alloc(m, sizeof(ClassDef));
    cls->type.of = TYPE_CLASS_DEF;
    cls->atts = Table_Make(m);
    cls->methods = Table_Make(m);
    cls->byKey = cls->byIdx = cls->getIter = ClassDef_Undefined;
    return cls;
}

status ClassDef_Init(MemCh *m){
    if(ClassLookup == NULL){
        ClassLookup = Lookup_Make(m, _TYPE_ZERO);
        return SUCCESS;
    }
    return NOOP;
}
