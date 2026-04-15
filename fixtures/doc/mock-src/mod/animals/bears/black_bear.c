/* Animals.bears.BlackBear
 *
 * Black bears are super feroucious
 */
#include <external.h>
#include <caneka.h>

void *Bear_Roar(Bear *b, i64 amount, status flags){
    return NULL;
}

void Bear_Run(Bear *b, i64 amount, status flags){
    return NULL;
}

Bear *Bear_Make(MemCh *m){
    Bear *b = MemCh_AllocOf(m, sizeof(Bear), TYPE_MOCK_BEAR);
    b->type.of = TYPE_MOCK_BEAR;
    return b;
}
