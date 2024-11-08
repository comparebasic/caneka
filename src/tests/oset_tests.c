#include <external.h>
#include <caneka.h>

status Oset_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = String_Make(m, bytes("hi:s/5=there;"));
    Abstract *a = Abs_FromOset(m, s);

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}
