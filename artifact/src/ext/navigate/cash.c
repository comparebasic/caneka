#include <external.h>
#include <caneka.h>

Cash *Cash_Make(MemCh *m){
    Cash *csh = MemCh_Alloc(m, sizeof(Cash));
    csh->type.of = TYPE_CASH;
    csh->p = Span_Make(m);
    return csh;
}

Cash *Cash_FromStrVec(MemCh *m, StrVec *v){
    Cash *csh = Cash_Make(m);
    Iter it;
    Iter_Init(&it, v->p){
        /* find $ and make cash objects in between strings */
    }
    return csh;
}
