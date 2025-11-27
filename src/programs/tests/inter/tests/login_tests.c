#include <external.h>
#include <caneka.h>

status Login_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    StrVec *absPath = IoAbsPath(m, "examples/session/open/");
    Str *path = StrVec_Str(m, absPath);
    SsidCtx *ctx = SsidCtx_Make(m, path);
    Dir_CheckCreate(m, path);

    StrVec *ua = Sv(m, "Firefudge/Aluminum");
    microTime time = MicroTime_Now();
    StrVec *ssid = Ssid_From(ctx, ua, time);
    Ssid_Open(ctx, ssid, ua);

    absPath = IoAbsPath(m, "examples/session/logins/");
    path = StrVec_Str(m, absPath);
    Dir_CheckCreate(m, path);

    StrVec *uid = Sv(m, "user-one");

    Login *lg = Login_Create(m, path, uid, ssid, NULL);

    args[0] = Seel_Get(lg, K(m, "errors"));
    args[1] = NULL;
    r |= TestShow((lg->type.state & (SUCCESS|ERROR)) == SUCCESS,
        "Login created without error",
        "Login not-created errors: @", args);

    StrVec_AddVec(absPath, uid);
    Str *loginPath = StrVec_Str(m, absPath);
    args[0] = loginPath;
    args[1] = NULL;
    r |= Test(Dir_Exists(m, loginPath) & SUCCESS, "User dir exists @", args);
    StrVec_Add(absPath, S(m, "/auth/"));
    loginPath = StrVec_Str(m, absPath);
    args[0] = loginPath;
    args[1] = NULL;
    r |= Test(Dir_Exists(m, loginPath) & SUCCESS, "User dir exists @", args);

    Login_Destroy(m, lg);
    Ssid_Destroy(ctx, ssid, ua);

    DebugStack_Pop();
    return r;
}
