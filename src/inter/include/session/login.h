typedef Inst Login;

status Login_Refresh(MemCh *m, Login *lg);
status Login_AddAuth(MemCh *m, Login *lg, Abstract *a);
Login *Login_Open(MemCh *m, Str *path, StrVec *uid, StrVec *ssid, AuthCred *cred);
Login *Login_Create(MemCh *m, Str *path, StrVec *uid,  StrVec *ssid, AuthCred *cred);
status Login_Persist(MemCh *m, Login *lg);
status Login_Destroy(MemCh *m, Login *lg);
status Login_Init(MemCh *m);
