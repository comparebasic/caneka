#include <external.h>
#include <caneka.h>
#include <test.h>
#include "tests.h"

status Core_Tests(MemCh *m){
    status r = READY;

    r |= Test(TRUE, "Test that a test can run", NULL);
    r |= Test(STR_MAX < MEM_SLAB_SIZE-(sizeof(slab)*(SPAN_MAX_DIMS-1)), "Test that the STR_MAX is less than a MemPage with 5 expansions span slabs", NULL);

    r |= SUCCESS;
    return r;
}
