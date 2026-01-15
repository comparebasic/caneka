#include <external.h>
#include <caneka.h>

status Itin_IterAdd(Iter *it, void *value){
    Abstract *a = (Abstract *)value;
    status r = Iter_Add(it, a);

    void *args[] = {value, NULL};
    Out("^p.Itin adding @^0\n", args);

    Single *sg = Lookup_Get(it->itin->positions, a->type.of);
    if(sg == NULL){
        sg = I32_Wrapped(it->p->m, it->idx);
        Lookup_Add(it->p->m, it->itin->positions, a->type.of, sg);
    }else{
        sg->val.i = it->idx;
    }
    return r;
}

void *Itin_GetByType(Iter *it, cls typeOf){
    i32 idx = it->idx;
    Single *sg = Lookup_Get(it->itin->positions, typeOf);
    void *value = NULL;
    if(sg != NULL){
        value = Iter_GetByIdx(it, sg->val.i);
    }
    Iter_GetByIdx(it, idx);
    return value;
}

Itin *Itin_Make(MemCh *m, cls typeOf){
    Itin *itin = MemCh_AllocOf(m, sizeof(Itin), TYPE_ITINERARY);
    itin->type.of = TYPE_ITINERARY;
    itin->objType.of = typeOf;
    itin->positions = Lookup_Make(m, ZERO);
    return itin;
}

void ItinIt_Init(Iter *it, Span *p){
    Iter_Init(it, p);
    it->itin = Itin_Make(p->m, p->type.of);
}

Iter *ItinIt_Make(MemCh *m, void *_a){
    Abstract *a = (Abstract *)_a;
    if(a->type.of != TYPE_SPAN &&
            a->type.of != TYPE_TABLE && (a->type.of & TYPE_INSTANCE) == 0){
        void *args[] = {Type_ToStr(m, a->type.of), NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Expected a Span like type of Inst", args);
        return NULL;
    }
    Iter *it = Iter_Make(m, (Span *)a);
    it->itin = Itin_Make(m, a->type.of);
    return it;
}
