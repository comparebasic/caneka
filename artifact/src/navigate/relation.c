#include <external.h>
#include <caneka.h>

Relation *Relation_Make(MemCh *m, i16 dims, i16 stride, Abstract **headers){
    Relation *rel = (Relation *)Span_Make(m);
    rel->type.of = TYPE_RELATION;
    rel->dims = dims;
    rel->stride = stride;
    rel->headers = headers;
    rel->p = Span_Make(m);
    return rel;
}

status Relation_HeadFromValues(Relation *rel){
    rel->headers = Span_ToArr(rel->p->m, rel->p);
    return Span_Wipe(rel->p);
}

status Relation_SetValue(Relation *rel, i16 col, i16 row, Abstract *value){
    return Span_Set(rel->p, row * rel->stride + col, value);
}
