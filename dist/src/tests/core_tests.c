#include <external.h>
#include <caneka.h>

status Core_Tests(MemCtx *gm){
    status r = READY;

    i64 mod = (STRING_CHUNK_SIZE+1) % SPAN_DEFAULT_SLOT_SIZE;
    r |= Test(mod == 0, "String size has no modulus for span default size, have %ld", mod);

    mod = (STRING_FIXED_SIZE+1) % SPAN_DEFAULT_SLOT_SIZE;
    r |= Test(mod == 0, "String fixed size has no modulus for span default size, have %ld", mod);

    mod = (STRING_FULL_SIZE+1) % SPAN_DEFAULT_SLOT_SIZE;
    r |= Test(mod == 0, "String full size has no modulus for span default size, have %ld", mod);

    r |= SUCCESS;
    return r;
}
