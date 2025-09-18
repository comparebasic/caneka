#include <external.h>
#include <caneka.h>

Span *Persist_GetSpan(MemCh *m){
    void *pg = Span_Get(m->it.p, 0);
    i64 offset = sizeof(MemCh)+sizeof(Span)+sizeof(slab);
    return (Span *)as((pg+MEM_SLAB_SIZE)-offset-sizeof(Span), TYPE_SPAN);
}

status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr){
    Ref *ref = Ref_Make(m);
    ref->coords.idx = slIdx;
    ref->coords.offset = (quad)(((util)ptr) & MEM_PERSIST_MASK);

    return Span_Add(Persist_GetSpan(m), (Abstract *)ref) >= 0 ?
        SUCCESS : ERROR;
}

MemCh *Persist_Make(){
    MemCh *m = MemCh_OnPage();
    Span_Make(m);
    m->type.of = TYPE_PERSIST;
    return m;
}
