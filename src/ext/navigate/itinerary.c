#include <external.h>
#include <caneka.h>

Itin *Itin_Make(MemCh *m, cls typeOf){
    Itin *itin = MemCh_AllocOf(m, sizeof(Itin), TYPE_ITINERARY);
    itin->type.of = TYPE_ITINERARY;
    itin->objType.of = typeOf;
    itin->positions = Lookup_Make(m, ZERO);
    return itin;
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
