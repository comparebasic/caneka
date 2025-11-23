#include <external.h>
#include <caneka.h>

status Session_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    
    microTime time = MicroTime_Now();
    StrVec *ua = Sv(m, "Firefudge/Aluminum");

    StrVec *absPath = IoAbsPath(m, "examples/session/open/");
    Str *path = StrVec_Str(m, absPath);
    SsidCtx *ctx = SsidCtx_Make(m, path);

    Dir_CheckCreate(m, path);

    StrVec *ssid = Ssid_From(ctx, ua, time);
    args[0] = ssid;
    args[1] = NULL;
    r |= Test(ssid->total == 52, "Ssid has length of 34, have @", args);
    quad parity = Parity_FromVec(ua);
    args[0] = Str_ToHex(m, Str_Ref(m, (byte *)&parity, sizeof(quad), sizeof(quad), STRING_BINARY));
    args[1] = Span_Get(ssid->p, 0);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "Ssid first seg is parity of User-Agent, expected @, have @", args);
    args[0] = Str_ToHex(m, Str_Ref(m, (byte *)&time, sizeof(microTime), sizeof(microTime), STRING_BINARY));
    args[1] = Span_Get(ssid->p, 2);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "Ssid second seg is time, expected @, have @", args);

    Ssid_Open(ctx, ssid, ua);

    StrVec *key = Sv(m, "username");
    Str *value = S(m, "fredieeee");
    Ssid_Set(ctx, ssid, key, value);
    StrVec *newSsid = Ssid_Update(ctx, ssid, ua, MicroTime_Now());

    StrVec *v = Ssid_Get(ctx, newSsid, key);

    args[0] = value;
    args[1] = v;
    r |= Test(Equals(value,v), "Value after update is a match @ @", args);

    r |= Test(Ssid_UnSet(ctx, newSsid, key) & SUCCESS,
        "Succsesfully removed username", NULL);

    r |= Test(Ssid_Close(ctx, newSsid, ua) & SUCCESS,
        "Succsesfully closed session", NULL);

    DebugStack_Pop();
    return r;
}
