#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Session_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[6];
    status r = READY;
    
    struct timespec time;
    Time_Now(&time);
    StrVec *ua = Sv(m, "Firefudge/Aluminum");

    StrVec *absPath = IoAbsPath(m, "fixtures/session/open/");
    Str *path = StrVec_Str(m, absPath);
    SsidCtx *ctx = SsidCtx_Make(m, path);
    Dir_CheckCreate(m, path);

    StrVec *ssid = Ssid_From(ctx, ua, &time);
    args[0] = ssid;
    args[1] = NULL;
    quad parity = HalfParity_FromVec(ua);
    args[0] = Str_ToHex(m, 
        Str_Ref(m, (byte *)&parity, sizeof(quad), sizeof(quad), STRING_BINARY));
    args[1] = Span_Get(ssid->p, 6);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "Ssid third seg is parity of User-Agent, expected @, have @", args);

    Str *timeSecS = Span_Get(ssid->p, 2);
    Str *timeNsecS = Span_Get(ssid->p, 4);
    struct timespec ssidTime;
    ssidTime.tv_sec = I64_FromStr(timeSecS);
    ssidTime.tv_nsec = I64_FromStr(timeNsecS);

    args[0] = Str_FromI64(m, time.tv_sec);
    args[1] = Str_FromI64(m, time.tv_nsec);
    args[2] = timeSecS;
    args[3] = timeNsecS;
    args[4] = Time_ToStr(m, &ssidTime);
    args[5] = NULL;
    r |= Test(ssidTime.tv_sec == time.tv_sec && ssidTime.tv_nsec == time.tv_nsec, "Ssid second seg is time, expected @.@, have @.@ ($)", 
        args);

    ssid = Ssid_Start(ctx, ua, &time);
    
    args[0] = ssid;
    args[1] = NULL;
    r |= Test(ssid != NULL, "Session ssid is not null after Start @", args);

    Table *stashTbl = Ssid_Open(ctx, ssid, ua);

    args[0] = stashTbl;
    args[1] = NULL;
    r |= Test(stashTbl != NULL, "Session data is not NULL @", args);

    Str *origSsid = Table_Get(stashTbl, K(m, "orig-ssid")); 
    r |= Test(Equals(origSsid, ssid), "Orig Ssid from stash equals ssid", NULL);

    StrVec *key = Sv(stashTbl->m, "username");
    Str *value = S(stashTbl->m, "fredieeee");
    Table_Set(stashTbl, key, value);
    Ssid_Close(ctx, ssid, ua, stashTbl);

    struct timespec ts;
    Time_Now(&ts);
    StrVec *newSsid = Ssid_Update(ctx, ssid, ua, &ts);
    Table *newTbl = Ssid_Open(ctx, newSsid, ua);

    args[0] = S(m, "fredieeee");
    args[1] = Table_Get(newTbl, S(m, "username"));
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "Username from updated session equals @, have @", 
        args);

    r |= Test(Ssid_Destroy(ctx, newSsid, ua) & SUCCESS,
        "Succsesfully closed session", NULL);

    DebugStack_Pop();
    return r;
}
