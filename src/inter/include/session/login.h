typedef Inst Login;

Login *Login_Open(MemCh *m, Str *path, StrVec *uid,  StrVec *ssid, AuthCred *cred);
Login *Login_Create(MemCh *m, Str *path, StrVec *uid,  StrVec *ssid, AuthCred *cred);
Login *Login_Persist(MemCh *m, Login *lg);
Login *Login_Refresh(MemCh *m, Login *lg);
status Login_Init(MemCh *m);
