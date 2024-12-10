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

    Span *data = Span_Make(m, TYPE_TABLE);
    Table_Set(data, (Abstract *)String_Make(m, bytes("email")), (Abstract *)email);

    Span *u = User_Open(m, users, userId, pass, ac, data);
    Debug_Print((void *)u, 0, "User: ", COLOR_PURPLE, TRUE);
    printf("\n");

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

    r |= Test(file.data->length == 231, "Auth Oset has expected length, have %d", file.data->length);

    Span *u2 = User_Open(m, users, userId, pass, ac, NULL);
    Debug_Print((void *)u2, 0, "User2: ", COLOR_PURPLE, TRUE);
    printf("\n");

    r |= Test(User_Delete(m, users, userId, ac) & SUCCESS, "User Delete has flag SUCCESS");

    MemCtx_Free(m);
    return r;
}
