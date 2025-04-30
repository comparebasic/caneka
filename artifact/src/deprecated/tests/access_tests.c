#include <external.h>
#include <caneka.h>

status Access_Tests(MemCtx *gm){
    DebugStack_Push("Crypto_KeyTests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Access *sys = Access_Make(m, String_Make(m, bytes("system")), NULL);
    Access *none = Access_Make(m, String_Make(m, bytes("none")), NULL);

    sys->type.state |= ACCESS_CREATE;
    r |= Test(GetAccess(sys, String_Make(m, bytes("grant"))) != NULL, 
        "sys has grant access");
    r |= Test(GetAccess(none, String_Make(m, bytes("grant"))) == NULL, 
        "none does not");

    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
