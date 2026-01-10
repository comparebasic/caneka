#include <external.h>
#include <caneka.h>

status IterUpper_FlagCombine(status base, status compare){
    status r = ZERO;

    if(base == compare){
        void *ar[] = {
            Type_StateVec(ErrStream->m, TYPE_ITER_UPPER,
                SUCCESS|UFLAG_ITER_STRICT),
            NULL
        };
        Out("^p.CombineResult @^0\n", ar);
        return SUCCESS|UFLAG_ITER_STRICT;
    }

    if(base == ZERO){
        void *ar[] = {
            Type_StateVec(ErrStream->m, TYPE_ITER_UPPER, ZERO),
            NULL
        };
        Out("^p.CombineResult @^0\n", ar);
        return FALSE;
    }

    status useful = UPPER_FLAGS & ~(UFLAG_ITER_STRICT|UFLAG_ITER_INVERT);
    void *_ar[] = {Type_StateVec(ErrStream->m, TYPE_ITER_UPPER, useful), NULL};
    Out("^p.Base @^0\n", _ar);

    status overlap = (useful & compare & base);
    
    boolean strict = overlap == (compare & useful);
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

    void *ar[] = {Type_StateVec(ErrStream->m, TYPE_ITER_UPPER, r), NULL};
    Out("^p.CombineResult @^0\n", ar);

    return r;
}
