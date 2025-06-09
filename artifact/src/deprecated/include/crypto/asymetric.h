status Asymetric_Verify(MemCh *m, StrVec *v, Str *sig, Secure *pubKey);
Str *Asymetric_Sign(MemCh *m, StrVec *v, Secure *Key);
Secure *Asymetric_VerifyInit(MemCh *m, StrVec *v, Str *sig, Secure *pubKey);
status Asymetric_VerifyUpdate(Secure *asym, Str *s);
Secure *Asymetric_VerifyFinal(Secure *asym, Str *s);
