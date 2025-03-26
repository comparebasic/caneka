#include <external.h>
#include <caneka.h>

status Core_Tests(MemCh *gm){
    status r = READY;

    MemCh *m = MemCh_Make();
    r |= Test(TRUE, "Test that a test can run");

    MemCh_Free(m);
    r |= SUCCESS;
    return r;
}
