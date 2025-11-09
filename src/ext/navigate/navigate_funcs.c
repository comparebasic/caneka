#include <external.h> 
#include <caneka.h> 

static Table *ordTableFuncs = NULL;

status Navigate_FuncInit(MemCh *m){
    if(FuncTableLookup == NULL){
        FuncTableLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return SUCCESS;
    }
    return NOOP;
}
