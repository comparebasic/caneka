typedef Inst Login;

Login *Login_Open(MemCh *m, StrVec *key, StrVec *cred, StrVec *ssid);
Login *Login_Persist(MemCh *m, Login *lg);
Login *Login_Refresh(MemCh *m, Login *lg);
status Login_Init(MemCh *m);
