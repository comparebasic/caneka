#include <external.h>
#include <caneka.h>

status Io_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    char *buff[1024];
    char *path = getcwd(buff, 1024);

    IoCtx *io = IoCtx_Make(m, String_Make(m, bytes(path), NULL, NULL));
    IoCtx *io = IoCtx_Make(m, String_Make(m, bytes(path), NULL, NULL));

    r |= Test(qidx->item == midwest, "midwest has replaed idx 17");

    MemCtx_Free(m);

    return r;
}

