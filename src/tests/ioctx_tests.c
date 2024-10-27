#include <external.h>
#include <caneka.h>

status IoCtx_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    IoCtx *root = IoCtx_MakeRoot(m, String_Make(m, bytes("tmp")), NULL);

    Debug_Print((void *)root->root, 0, "", COLOR_PURPLE, TRUE);
    printf("\n");
    /*
    IoCtx *ctx = IoCtx_Make(m, String_Make(m, bytes("ctx-alpha")), NULL, root);
    */

    MemCtx_Free(m);
    return SUCCESS;
}
