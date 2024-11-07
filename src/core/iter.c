#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    if(it->type.state == END){
        it->idx = -1;
        it->type.state &= ~END;
    }
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

Iter *Iter_Init(Iter *it, Span *values){
    it->type.of = TYPE_ITER;
    it->values = values;
    it->idx = -1;
    return it;
}

Iter *Iter_Make(MemCtx *m, Span *values){
    Iter *it = (Iter *)MemCtx_Alloc(m, sizeof(Iter));
    Iter_Init(it, values);
    return it;
}
