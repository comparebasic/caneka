#include <external.h>
#include <caneka.h>

status Core_Tests(MemCtx *gm){
    status r = READY;

    MemCtx *m = MemCtx_Make();
    r |= Test(TRUE, "Test that a test can run");

    MemCtx_Free(m);
    r |= SUCCESS;
    return r;
}
