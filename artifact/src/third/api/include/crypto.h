Str *Str_ToSha256(MemCh *m, Str *s);
Str *StrVec_ToSha256(MemCh *m, StrVec *v);
Str *Str_SaltedDigest(MemCh *m, Str *s, Str *salt);
status KeyPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase);
StrVec *KeyPair_Sign(MemCh *m, Str *secret, StrVec *content);
status KeyPair_Verify(MemCh *m, Str *public, StrVec *content);
status Str_HmacEnc(MemCh *m, Str *s, Str *hmac);
status Str_HmacDec(MemCh *m, Str *s, Str *hmac);
