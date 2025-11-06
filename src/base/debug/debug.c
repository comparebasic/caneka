#include <external.h>
#include <caneka.h>

Lookup *TypeStringRanges = NULL;

status Debug_Init(MemCh *m){
    if(TypeStringRanges == NULL){
        TypeStringRanges = Lookup_Make(m, _TYPE_ZERO);
        DebugTypeStrings_Init(m);
    }
    m->level++;
    return NOOP;
}
