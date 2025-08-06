#include <external.h>
#include <caneka.h>

Lookup *ClassLookup = NULL;

ClassDef *ClassDef_Make(MemCh *m){
    ClassDef *cls = (ClassDef*)MemCh_Alloc(m, sizeof(ClassDef));
    cls->type.of = TYPE_CLASS_DEF;
    cls->attsTbl = Table_Make(m);
    cls->methodsTbl = Table_Make(m);
    return cls;
}

status ClassDef_Init(MemCh *m){
    if(ClassLookup == NULL){
        ClassLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return SUCCESS;
    }
    return NOOP;
}
