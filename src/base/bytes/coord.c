#include <external.h>
#include "base_module.h"

Coord *Coord_Make(MemCh *m, word a, word b){
    Coord *cr = (Coord *)MemCh_Alloc(m, sizeof(Coord));
    cr->type.of = TYPE_COORD;
    cr->a = a;
    cr->b = b;
    return cr;
}

StrVec *StrVec_CoordCopy(MemCh *m, StrVec *v, Coord *cr){
    Iter it;
    Iter_Init(&it, v->p);
    Iter_GetByIdx(&it, (i32)cr->a);
    it.type.state |= PROCESSING;
    StrVec *ret = StrVec_Make(m);
    Str *s = (Str *)Iter_Get(&it);
    StrVec_Add(ret, s);
    while((Iter_Next(&it) & END) == 0 && it.idx <= (i32)cr->b){
        Str *s = (Str *)Iter_Get(&it);
        StrVec_Add(ret, s);
    }
    return ret;
}
