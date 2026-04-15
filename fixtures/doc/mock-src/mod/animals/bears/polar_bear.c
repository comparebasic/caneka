/* Animals.bears.PolarBear
 *
 * Polar bears are super feroucious
 */
#include <external.h>
#include <caneka.h>

void *Polar_Roar(Bear *b, i64 amount, status flags){
    return NULL;
}

void Polar_Run(Bear *b, i64 amount, status flags){
    return NULL;
}

Bear *Polar_Make(MemCh *m){
    Bear *b = MemCh_AllocOf(m, sizeof(Bear), TYPE_MOCK_POLAR_BEAR);
    b->type.of = TYPE_MOCK_POLAR_BEAR;
    return b;
}
