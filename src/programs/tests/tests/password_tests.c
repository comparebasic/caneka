#include <external.h>
#include <caneka.h>

status Password_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    DebugStack_SetRef(S(m, "Password NoWrap"), TYPE_STR);
    Str *pw = S(m, "$%_{}@!@-=+#");
    Str *salt = Test_GetStr512(m);

    Str *hash = Str_Make(m, DIGEST_SIZE);
    Password_OnStr(m, hash, pw, salt);
    Str *digest = Str_ToHex(m, hash);
    Str *expected = S(m,
        "4e732bfa69b00e0dfc60ff488786b723f27d640f72598ba3b8b1adb579cfdf69");
    args[0] = expected;
    args[1] = digest;
    args[2] = NULL;
    r |= Test(Equals(digest, expected), "Password bytes are expected @, have @", args);

    DebugStack_SetRef(S(m, "Password WrapAround"), TYPE_STR);
    pw = S(m, "\x96\xff$%_{}&!@-=+#/");

    hash = Str_Make(m, DIGEST_SIZE);
    Password_OnStr(m, hash, pw, salt);
    digest = Str_ToHex(m, hash);
    expected = S(m,
        "b32465d79219a499a6abc69060d63fc03262354727cf7ad16df667185247eeff");
    args[0] = expected;
    args[1] = digest;
    args[2] = NULL;
    r |= Test(Equals(digest, expected), "Password bytes are expected @, have @", args);

    DebugStack_Pop();
    return r;
}
