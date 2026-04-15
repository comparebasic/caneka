/* Animals.bears.GrizzlyBear
 *
 * Grizzly bears are super feroucious
 */
#include <external.h>
#include <caneka.h>

void *Grizzly_Roar(Bear *b, i64 amount, status flags){
    return NULL;
}

void Grizzly_Run(Bear *b, i64 amount, status flags){
    return NULL;
}

Bear *Grizzly_Make(MemCh *m){
    Bear *b = MemCh_AllocOf(m, sizeof(Bear), TYPE_MOCK_GRIZZLY_BEAR);
    b->type.of = TYPE_MOCK_GRIZZLY_BEAR;
    return b;
}
