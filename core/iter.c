#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    if((it->idx+1) > it->values->max_idx){
        it->type.state = END;
    }else{
        if(it->type.state != READY){
            it->idx++;
        }
        it->type.state = SUCCESS;
    }
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    return Span_Get(it->values, it->idx);
}

Iter *Iter_Make(MemCtx *m, Span *values){
    Iter *it = (Iter *)MemCtx_Alloc(m, sizeof(Iter));
    it->type.of = TYPE_ITER;
    it->values = values;
    return it;
}
