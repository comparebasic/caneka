#include <external.h>
#include <caneka.h>

status User_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    IoCtx *root = IoCtxTests_GetRootCtx(m);
    /* add test key */
    Access *ac = Access_Make(m, Cont(m, bytes("test")), NULL);
    Access *system = Access_Make(m, Cont(m, bytes("system")), NULL);

    Span *keys = Span_Make(m, TYPE_TABLE);
    Table_Set(keys, (Abstract *)Cont(m, bytes("test")),
        (Abstract *)Salty_MakeKey(m, Cont(m, bytes("Tests are Fun Fun Fun!"))));

    status keysAdded = EncPair_AddKeyTable(m, keys, system);
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

    Span *u = User_Open(m, users, userId, pass, ac);

    MemCtx_Free(m);
    return r;
}
