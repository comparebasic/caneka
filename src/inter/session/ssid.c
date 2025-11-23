#include <external.h>
#include <caneka.h>

StrVec *Ssid_From(SsidCtx *ctx, StrVec *ua, microTime time){
    MemCh *m = ctx->m;
    quad parity = Parity_FromVec(ua);
    StrVec *v = StrVec_Make(m);

    StrVec_Add(v,
        Str_ToHex(m,
            Str_Ref(m, (byte *)&parity, sizeof(parity), sizeof(parity), STRING_BINARY)));

    StrVec_Add(v, S(m, "-"));
    StrVec_Add(v,
        Str_ToHex(m,
            Str_Ref(m, (byte *)&time, sizeof(time), sizeof(time), STRING_BINARY)));

    StrVec_Add(v, S(m, "-"));
    Str *rand = Str_Make(m, 8);
    Buff_GetStr(RandStream, rand);
    StrVec_Add(v, Str_ToHex(m, rand));

    StrVec_Add(v, S(m, "-"));
    StrVec_Add(v, Str_FromI64(m, ctx->metrics.count++));

    return v;
}

status Ssid_Open(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    return ZERO;
}

StrVec *Ssid_Update(SsidCtx *ctx, StrVec *ssid, microTime time, StrVec *value){
    return NULL;
}

StrVec *Ssid_Start(SsidCtx *ctx, StrVec *ua, microTime time, StrVec *value){
    return NULL;
}

status Ssid_Close(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    return NULL;
}

SsidCtx *SsidCtx_Make(m, Str *path){
    return NULL;
}
