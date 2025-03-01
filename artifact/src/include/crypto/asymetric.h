status Asymetric_Verify(MemCtx *m, String *s, String *sig, Secure *pubKey);
String *Asymetric_Sign(MemCtx *m, String *s, Secure *Key);
Secure *Asymetric_VerifyInit(MemCtx *m, String *s, String *sig, Secure *pubKey);
status Asymetric_VerifyUpdate(Secure *asym, String *s);
Secure *Asymetric_VerifyFinal(Secure *asym, String *s);
