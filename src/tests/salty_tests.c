#include <external.h>
#include <caneka.h>

status Salty_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *phrase = CB_Phrase(m);
    Debug_Print((void *)phrase, 0, "Phrase: ", COLOR_PURPLE, TRUE);
    printf("\n");

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

