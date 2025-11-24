#include <external.h>
#include <caneka.h>

Login *Login_Open(MemCh *m, StrVec *key, StrVec *cred, StrVec *ssid);
Login *Login_Persist(MemCh *m, Login *lg);
Login *Login_Refresh(MemCh *m, Login *lg);

status Login_Init(MemCh *m){
    status r = READY;
    Table *seel = Table_Make(m);
    Table_Set(seel, S(m, "uid"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "email"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "auth"), I16_Wrapped(m, TYPE_TABLE));
    Hashed *h = Table_SetHashed(seel, S(m, "content"), I16_Wrapped(m, TYPE_TABLE));
    r |= Seel_Seel(m, seel, S(m, "Login"), TYPE_LOGIN, h->orderIdx);
    return r;
}
