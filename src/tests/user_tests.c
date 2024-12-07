#include <external.h>
#include <caneka.h>

status User_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    IoCtx *root = IoCtxTests_GetRootCtx(m);
    /* add test key */
    Access *ac = Access_Make(m, Cont(m, bytes("test")), NULL);
    Tests_AddTestKey(gm);
    /* end add test key */

    String *users_s = String_Make(m, bytes("users"));
    IoCtx *users = IoCtx_Make(m, users_s, NULL, root);
    IoCtx_Persist(m, users);

    String *session_s = String_Make(m, bytes("session"));
    IoCtx *session = IoCtx_Make(m, session_s, NULL, root);
    IoCtx_Persist(m, session);

    String *email =  String_Make(m, bytes("me@example.com"));
    String *userId = String_ToHex(m, String_Sha256(m, email));

    String *pass =  String_Make(m, bytes("Bork_Bork128!Bork"));

    Span *u = User_Open(m, users, userId, pass, ac, NULL);

    MemCtx_Free(m);
    
    r |= SUCCESS;

    return r;
}
