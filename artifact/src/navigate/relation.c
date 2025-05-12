#include <external.h>
#include <caneka.h>

status Relation_HeadFromValues(Relation *rel){
    status r = READY;
    rel->stride = rel->it.p->nvalues;
    rel->headers = Span_ToArr(rel->it.p->m, rel->it.p);
    r |= Span_Wipe(rel->it.p);
    Iter_Setup(&rel->it, rel->it.p, SPAN_OP_SET, 0);
    return r;
}

status Relation_Start(Relation *rel){
    Iter_Setup(&rel->it, rel->it.p, SPAN_OP_SET, 0);
    return SUCCESS;
}

status Relation_Next(Relation *rel){
    status r = READY;
    r = Iter_Next(&rel->it);
    if((r & END) == 0){
        i32 col = rel->it.idx % rel->stride;
        rel->type.state &= NORMAL_FLAGS;
        if(col == 0){
            rel->type.state |= RELATION_ROW_START;
        }else if(col == 1){
            rel->type.state |= RELATION_ROW_END;
        }
        r |= rel->type.state;
    }
    return r;
}

status Relation_SetValue(Relation *rel, i16 col, i16 row, Abstract *value){
    Iter_Setup(&rel->it, rel->it.p, SPAN_OP_SET, row * rel->stride + col);
    rel->it.value = value;
    return Iter_Query(&rel->it);
}

Relation *Relation_Make(MemCh *m, i16 stride, Abstract **headers){
    Relation *rel = (Relation *)MemCh_Alloc(m, sizeof(Relation));
    rel->type.of = TYPE_RELATION;
    rel->stride = stride;
    rel->headers = headers;
    Span *p = Span_Make(m);
    Iter_Setup(&rel->it, p, SPAN_OP_SET, 0);
    return rel;
}
