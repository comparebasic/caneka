#include <external.h>
#include <caneka.h>

status Span_Set(Span *p, i32 idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NOOP;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    it.value = (void *)t;
    status r = Iter_Query(&it);
    if(p->type.state & DEBUG){
        printf("\x1b[34mSpan_Set %d\x1b[0m\n", p->nvalues);
    }
    return r;
}

void *Span_Get(Span *p, i32 idx){
    if(idx < 0){
        return NULL;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_GET, idx);
    if(Iter_Query(&it) & SUCCESS){
        return it.value;
    }
    return NULL;
}

status Span_Remove(Span *p, i32 idx){
    if(idx < 0 ){
        return NOOP;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_REMOVE, idx);
    it.value = (void *)NULL;
    return Iter_Query(&it);
}

status Span_Setup(Span *p){
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    return SUCCESS;
}

Span *Span_Make(MemCh *m){
    Span *p = MemCh_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    p->m = m;
    p->root = (slab *)MemCh_Alloc((m), sizeof(slab));
    return p;
}
