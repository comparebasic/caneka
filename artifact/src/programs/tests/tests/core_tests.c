#include <external.h>
#include <caneka.h>

status Core_Tests(MemCh *gm){
    status r = READY;

    MemCh *m = MemCh_Make();

    r |= Test(TRUE, "Test that a test can run", NULL);
    r |= Test(STR_MAX < MEM_SLAB_SIZE-(sizeof(slab)*(SPAN_MAX_DIMS-1)), "Test that the STR_MAX is less than a MemPage with 5 expansions span slabs", NULL);

    MemCh_Free(m);
    r |= SUCCESS;
    return r;
}
