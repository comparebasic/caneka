#include <external.h>
#include <caneka.h>

Login *Login_Open(MemCh *m, Str *path, StrVec *uid, StrVec *ssid, AuthCred *cred){
    Login *lg = Inst_Make(m, TYPE_LOGIN);
    Seel_Set(lg, K(m, "path"), path); 
    Seel_Set(lg, K(m, "uid"), uid); 
    Seel_SetKv(lg, K(m, "auth"), S(m, "login"), cred); 

    Str *loginPathV = StrVec_Make(m);
    StrVec_Add(loginPathV, path);
    StrVec_Add(loginPathV, uid);

    Str *loginPath = StrVec_Str(m, loginPathV);
    if(!Dir_Exists(m, loginPath)){
        Seel_SetKv(lg, K(m, "errors"), S(m, "not-found"), S(m, "Login not found")); 
        lg->type.state |= ERROR;
        return lg;
    }

    StrVec_Add(loginPathV, S(m, "/"));
    Table *seel = Lookup_Get(SeelLookup, lg->type.of);
    Iter it;
    Iter_Init(&it, seel);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Single *prop = h->value;
            if(h->orderIdx > 3 && prop->val.w == TYPE_STRVEC){
                StrVec_Add(loginPathV, h->key);
                StrVec *v = File_ToVec(m, StrVec_Str(m, loginPathV));
                if(v != NULL){
                    Seel_Set(lg, h->key, v);
                }
            }
        }
    }

    /* gather auth */


    lg->type.state |= SUCCESS;
    return lg;
}

Login *Login_Create(MemCh *m, Str *path, StrVec *uid,  StrVec *ssid, AuthCred *cred){
    ;
}

Login *Login_Persist(MemCh *m, Login *lg);
Login *Login_Refresh(MemCh *m, Login *lg);

status Login_Init(MemCh *m){
    status r = READY;
    Table *seel = Table_Make(m);
    Table_Set(seel, S(m, "path"), I16_Wrapped(m, TYPE_STR));
    Table_Set(seel, S(m, "uid"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "auth"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(seel, S(m, "errors"), I16_Wrapped(m, TYPE_TABLE));
    /* end required fields */
    Table_Set(seel, S(m, "email"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "username"), I16_Wrapped(m, TYPE_STRVEC));
    Hashed *h = Table_SetHashed(seel, S(m, "content"), I16_Wrapped(m, TYPE_TABLE));
    r |= Seel_Seel(m, seel, S(m, "Login"), TYPE_LOGIN, h->orderIdx);
    return r;
}
