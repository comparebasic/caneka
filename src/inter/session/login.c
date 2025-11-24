#include <external.h>
#include <caneka.h>

status Login_Refresh(MemCh *m, Login *lg){
    Str *path = Seel_Get(lg, K(m, "path")); 
    StrVec *uid = Seel_Get(lg, K(m, "uid")); 

    StrVec *loginPathV = StrVec_Make(m);
    StrVec_Add(loginPathV, path);
    StrVec_AddVec(loginPathV, uid);

    StrVec_Add(loginPathV, S(m, "/"));
    Table *seel = Lookup_Get(SeelLookup, lg->type.of);
    Iter it;
    Iter_Init(&it, seel);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Single *prop = h->value;
            if(prop->val.w == TYPE_STRVEC || prop->val.w == TYPE_STR){
                StrVec_Add(loginPathV, h->key);
                StrVec *v = File_ToVec(m, StrVec_Str(m, loginPathV));
                if(v != NULL){
                    Abstract *a = (Abstract *)v;
                    if(prop->val.w == TYPE_STR){
                        a = (Abstract *)StrVec_Str(m, v);
                    }
                    Seel_Set(lg, h->key, a);
                }
                StrVec_Pop(loginPathV);
            }
        }
    }

    StrVec_Add(loginPathV, K(m, "auth/"));
    Str *loginPath = StrVec_Str(m, loginPathV);
    Span *authFiles = Span_Make(m);
    if(!Dir_Exists(m, loginPath)){
        Dir_Gather(m, loginPath, authFiles);
    }

    if(authFiles->nvalues > 0){
        /* handle auth files here */ 
    }

    return SUCCESS;
}

status Login_AddAuth(MemCh *m, Login *lg, Abstract *a){
    return ZERO; 
}

Login *Login_Open(MemCh *m, Str *path, StrVec *uid, StrVec *ssid, AuthCred *cred){
    Login *lg = Inst_Make(m, TYPE_LOGIN);
    Seel_Set(lg, K(m, "path"), path); 
    Seel_Set(lg, K(m, "uid"), uid); 
    Seel_SetKv(lg, K(m, "auth"), S(m, "login"), cred); 

    StrVec *loginPathV = StrVec_Make(m);
    StrVec_Add(loginPathV, path);
    StrVec_AddVec(loginPathV, uid);

    Str *loginPath = StrVec_Str(m, loginPathV);
    if(!Dir_Exists(m, loginPath)){
        Seel_SetKv(lg, K(m, "errors"), S(m, "not-found"), S(m, "Login not found")); 
        lg->type.state |= ERROR;
        return lg;
    }


    lg->type.state |= SUCCESS;
    return lg;
}

Login *Login_Create(MemCh *m, Str *path, StrVec *uid,  StrVec *ssid, AuthCred *cred){
    Login *lg = Inst_Make(m, TYPE_LOGIN);
    Seel_Set(lg, K(m, "path"), path); 
    Seel_Set(lg, K(m, "uid"), uid); 
    Seel_SetKv(lg, K(m, "auth"), S(m, "login"), cred); 

    StrVec *loginPathV = StrVec_Make(m);
    StrVec_Add(loginPathV, path);
    StrVec_AddVec(loginPathV, uid);

    Str *loginPath = StrVec_Str(m, loginPathV);
    if(Dir_Exists(m, loginPath)){
        Seel_SetKv(lg,
            K(m, "errors"),
            S(m, "already-exists"),
            S(m, "Existing user directory found")
        );
        lg->type.state |= ERROR;
        return lg;
    }

    StrVec_Add(loginPathV, S(m, "/"));

    StrVec_Add(loginPathV, K(m, "auth/"));
    loginPath = StrVec_Str(m, loginPathV);
    Dir_Mk(m, loginPath);

    lg->type.state |= Login_Refresh(m, lg);
    return lg;
}

status Login_Persist(MemCh *m, Login *lg){
    Str *path = Seel_Get(lg, K(m, "path")); 
    StrVec *uid = Seel_Get(lg, K(m, "uid")); 

    StrVec *loginPathV = StrVec_Make(m);
    StrVec_Add(loginPathV, path);
    StrVec_AddVec(loginPathV, uid);

    Str *loginPath = StrVec_Str(m, loginPathV);
    if(!Dir_Exists(m, loginPath)){
        Seel_SetKv(lg,
            K(m, "errors"),
            S(m, "not-found"),
            S(m, "User directory not found")
        ); 
        lg->type.state |= ERROR;
        return lg->type.state;
    }

    StrVec_Add(loginPathV, S(m, "/"));
    Table *seel = Lookup_Get(SeelLookup, lg->type.of);
    Iter it;
    Iter_Init(&it, seel);
    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Single *prop = h->value;
            if(prop->val.w == TYPE_STRVEC || prop->val.w == TYPE_STR){
                Abstract *a = (Abstract *)Seel_Get(lg, h->key);
                if(a != NULL){
                    StrVec_Add(loginPathV, h->key);
                    File_Open(bf, StrVec_Str(m, loginPathV), O_CREAT|O_TRUNC|O_WRONLY);
                    ToS(bf, a, a->type.of, ZERO);
                    File_Close(bf);
                    StrVec_Pop(loginPathV);
                }
            }
        }
    }

    return SUCCESS;
}

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
