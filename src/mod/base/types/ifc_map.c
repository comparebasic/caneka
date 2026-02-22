#include <external.h>
#include "base_module.h"

Lookup *ConvLookup;
Lookup *IfcLookup;

void *Coerce(MemCh *m, void *_a, cls typeOf){
    Abstract *a = (Abstract *)_a;
    if(Lookup_Get(IfcLookup, a->type.of)){
        ;
    }
}

void CoerceInit(MemCh *m){
    if(ConvLookup == NULL){
        ConvLookup = Lookup_Make(m, ZERO);
        IfcLookup = Lookup_Make(m, ZERO);
    }
}
