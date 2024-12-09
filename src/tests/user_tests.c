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

    String *abs = Buff(m, NULL);
    String_Add(m, abs, users->abs);
    String_AddBytes(m, abs, bytes("/"), 1);
    String_Add(m, abs, userId);
    String_AddBytes(m, abs, bytes("/"), 1);
    String_AddBytes(m, abs, bytes("password.auth"), strlen("password.auth"));

    File file;
    File_Init(&file, abs, NULL, NULL);
    file.abs = file.path;
    File_Load(m, &file, ac);

    char *exp = "auth/2={salt:enc/3={key:s/4=test;enc:s/226=05c5aa8d72060d43f8365ac92ad9c101b40c849ed1b7a9ec656ea50cd3afc4c08d80cfbbeae4ab6893748912589d33a0a47ef1b8af385c7aabe6d0dd5ad9fbfff763cec3367c3ef2a20082aa4431b85176d9b370172f598bff24db36ffd08f46f69653e5a25a84b8e81c16d6b9dd6fb0d1;dec:x/0=;}digest:s/64=838bf7f8636e6a279c269260f61e31a8c55bcd93856310b5c61942ff2fa08ce9;}";
    r |= Test(file.data->length == 231, "Auth Oset has expected length, have %d", file.data->length);

    r |= Test(User_Delete(m, users, userId, ac) & SUCCESS, "User Delete has flag SUCCESS");

    MemCtx_Free(m);
    return r;
}
