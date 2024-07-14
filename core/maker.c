#include <external.h>
#include <caneka.h>

Abstract *Maker_Make(MemCtx*m, void *mk, cls type){
    MakerObj *v = (MakerObj *)MemCtx_Alloct(m, sizeof(MakerObj));
    v->type.of = TYPE_MAKER;
    v->type.state = type;
    v->ptr = (util)mk;
    return v;
}

