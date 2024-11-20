#include <external.h>
#include <caneka.h>

status Iter_Next(Iter *it){
    if(it->type.state == END){
        it->idx = 0;
        it->type.state &= ~(END|FLAG_ITER_LAST);
    }
    if((it->idx+1) > it->values->max_idx){
        it->type.state = END;
    }else{
        it->idx++;
        it->type.state = SUCCESS;
        if(it->idx == it->values->max_idx){
            it->type.state |= FLAG_ITER_LAST;
        }

    }
    return it->type.state;
}

Abstract *Iter_Get(Iter *it){
    return Span_Get(it->values, it->idx);
}

Iter *Iter_Init(Iter *it, Span *values){
    memset(it, 0, sizeof(Iter));
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
