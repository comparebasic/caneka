#include <external.h>
#include <caneka.h>

status Roebling_SyntaxTests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    MemCtx_Free(m);
    return r;
}

