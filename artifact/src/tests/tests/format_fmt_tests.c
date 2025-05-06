#include <external.h>
#include <caneka.h>

status Roebling_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    MemCh_Free(m);
    return r;
}
