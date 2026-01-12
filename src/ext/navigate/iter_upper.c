#include <external.h>
#include <caneka.h>

status IterUpper_FlagCombine(status base, status compare){
    status r = ZERO;

    if(base == compare){
        return SUCCESS|UFLAG_ITER_STRICT;
    }

    if(base == ZERO){
        return FALSE;
    }

    status useful = UPPER_FLAGS & ~(UFLAG_ITER_STRICT|UFLAG_ITER_INVERT);
    status overlap = (useful & compare & base);
    boolean strict = (base & useful) == (compare & useful);

    boolean match = (base & UFLAG_ITER_STRICT) ?
        strict:
        overlap != 0;

    boolean invert = ((base & UFLAG_ITER_INVERT) != 0);
    if(match != invert){
        r |= SUCCESS;
    }else{
        r |= NOOP;
    }

    if(strict){
        r |= UFLAG_ITER_STRICT;
    }

    return r;
}
