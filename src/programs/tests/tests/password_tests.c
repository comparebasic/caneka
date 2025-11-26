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
        "201246817fa75057c8c6ea1dea13649a8d2d843b94a365f5c93ae9f8d90007f3");
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
        "35d92737d6efcf8d70f881ba485cc9252f0e12be3c966747df774e268cc1a424");
    args[0] = expected;
    args[1] = digest;
    args[2] = NULL;
    r |= Test(Equals(digest, expected), "Password bytes are expected @, have @", args);

    DebugStack_Pop();
    return r;
}
