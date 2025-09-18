#include <external.h>
#include <caneka.h>

Ref *Ref_Make(MemCh *m){
    Ref *rf = MemCh_Alloc(m, sizeof(Ref));
    rf->type.of = TYPE_REF;
    return rf;
}
