#define DIGEST_SIZE 32
#define SIG_FOOTER_SIZE 64
#define SIGNATURE_SIZE 96
status Str_ToSha256(MemCh *m, Str *s, digest *hash);
status StrVec_ToSha256(MemCh *m, StrVec *v, digest *hash);
status StrVec_SaltedDigest(MemCh *m, StrVec *v, Str *salt, digest *hash);
status SignPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase);
Str *SignPair_Sign(MemCh *m, Str *content, Str *secret);
status SignPair_Verify(MemCh *m, Str *content, Str *sig, Str *public);
status BoxPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase);
status BoxPair_Enc(MemCh *m, Str *secret, StrVec *content);
status BoxPair_Dec(MemCh *m, Str *public, StrVec *content);
status Str_HmacEnc(MemCh *m, Str *s, Str *hmac);
status Str_HmacDec(MemCh *m, Str *s, Str *hmac);
Str *Str_DigestAlloc(MemCh *m);
