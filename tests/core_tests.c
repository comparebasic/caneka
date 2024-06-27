#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Core_Tests(MemCtx *gm){
    status r = READY;
    r |= Test(sizeof(StringMin) == sizeof(Abstract *)*8, 
        "Expect StringMin to have 8 Abstract footprints %lu found %lu", sizeof(StringMin), sizeof(Abstract)*8);
    r |= Test(sizeof(StringMin) < SLAB_BYTE_SIZE, 
        "Expect StringMin to have a smaller footprint than a slab %lu found %lu", SLAB_BYTE_SIZE, sizeof(StringMin));
    r |= Test(sizeof(String) == SLAB_BYTE_SIZE, 
        "Expect String equal footprint than a slab %lu found %lu", SLAB_BYTE_SIZE, sizeof(String));
    return r;
}
