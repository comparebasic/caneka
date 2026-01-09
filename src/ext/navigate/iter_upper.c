#include <external.h>
#include <caneka.h>

boolean IterUpper_FlagCombine(status base, status compare){
    if(base == ZERO){
        return FALSE;
    }
    status overlap = (base & compare & (~UFLAG_ITER_STRICT));
    boolean match = (base & UFLAG_ITER_STRICT) && overlap == base || overlap != 0;
    boolean invert = (base & UFLAG_ITER_INVERT);
    printf("FlagCombine? %d\n", match != invert);
    return match != invert;
}
