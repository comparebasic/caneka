#include <external.h>
#include <caneka.h>

status IterUpper_FlagCombine(status base, status compare){
    status r = ZERO;
    if(base == ZERO){
        return FALSE;
    }
    status overlap = (base & compare & (~(UFLAG_ITER_STRICT|UFLAG_ITER_INVERT)));
    boolean match = (base & UFLAG_ITER_STRICT) && overlap == base ||
        ((base & UFLAG_ITER_STRICT) == 0 && overlap != 0);
    boolean invert = (base & UFLAG_ITER_INVERT) != 0;
    printf("FlagCombine? %d %d = %d\n", match, invert, match != invert);
    if(match != invert){
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }
    return r;
}
