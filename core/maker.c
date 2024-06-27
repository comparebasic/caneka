#include <external.h>
#include <caneka.h>

Abstract *Maker_Make(MemCtx*m, void *mk, cls type){
    Abstract *v = (Vitrual *)MemCtx_Alloct(m, sizeof(Abstract));
    v->type.of = TYPE_MAKER;
    v->type.state = type;
    v->ptr = (util)mk;
    return v;
}

