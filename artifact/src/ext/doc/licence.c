#include <external.h>
#include <caneka.h>

Lookup *LicenceLookup = NULL;

status Caneka_LicenceInit(MemCh *m){
    if(LicenceLookup == NULL){
        LicenceLookup = Lookup_Make(m, _TYPE_ZERO);
        return SUCCESS;
    }
    return NOOP;
}
