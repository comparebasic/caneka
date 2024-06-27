#include <external.h>
#include <caneka.h>

Virtual *Maker_Make(MemCtx*m, void *mk, cls type){
    Virtual *v = (Vitrual *)MemCtx_Alloct(m, sizeof(Virtual));
    v->type.of = TYPE_MAKER;
    v->type.state = type;
    v->ptr = (util)mk;
    return v;
}

