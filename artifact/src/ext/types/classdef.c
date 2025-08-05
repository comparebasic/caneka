#include <external.h>
#include <caneka.h>

extern Lookup *ClassLookup;

ClassDef *ClassDef_Make(MemCh *m){
    ClassDef *cls = (ClassDef*)MemCh_Alloc(m, sizeof(ClassDef));
    cls->type.of = TYPE_CLASS_DEF;
    cls->attsTbl = Table_Make(m);
    cls->methodsTbl = Table_Make(m);
    return cls;
}
