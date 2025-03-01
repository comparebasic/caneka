String *Digest_Sha256(MemCtx *m, String *s);
struct secure *Digest_Sha256Init(MemCtx *m);
status Digest_Sha256Update(struct secure *sha, String *s);
String *Digest_Sha256Final(MemCtx *m);
