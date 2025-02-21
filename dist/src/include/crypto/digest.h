String *Digest_Sha256(MemCtx *m, String *s);
String *Digest_Sign(MemCtx *m, String *content, String *key);
String *Digest_Verify(MemCtx *m, String *content, String *pubKey);
