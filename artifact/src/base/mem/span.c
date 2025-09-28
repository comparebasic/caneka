#include <external.h>
#include <caneka.h>

i32 dim_max_idx[TABLE_MAX_DIMS] = {15, 255, 4095, 65535, 1048575};

status Span_Set(Span *p, i32 idx, Abstract *t){
    if(idx < 0){
        return NOOP;
    }
    Iter it;
    Iter_Init(&it, p);
    return Iter_SetByIdx(&it, idx, (void *)t);
}

boolean Span_IsBlank(Span *p){
    return p->nvalues == 0;
}

void *Span_Get(Span *p, i32 idx){
    if(idx < 0){
        return NULL;
    }
    Iter it;
    Iter_Init(&it, p);
    return Iter_GetByIdx(&it, idx);
}

status Span_Remove(Span *p, i32 idx){
    if(idx < 0 ){
        return NOOP;
    }
    Iter it;
    Iter_Init(&it, p);
    return Iter_RemoveByIdx(&it, idx);
}

status Span_Setup(Span *p){
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    return SUCCESS;
}

Span *Span_Make(MemCh *m){
    Span *p = MemCh_AllocOf(m, sizeof(Span), TYPE_SPAN);
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    p->m = m;
    p->root = (slab *)Bytes_Alloc((m), sizeof(slab));
    return p;
}
