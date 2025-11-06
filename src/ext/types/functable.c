#include <external.h>
#include <caneka.h>

Lookup *FuncTableLookup;

status FuncTable_Init(MemCh *m){
    if(FuncTableLookup == NULL){
        FuncTableLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return SUCCESS;
    }
    return NOOP;
}
