Str *Digest_Sha256(MemCh *m, StrVec *v);
struct secure *Digest_Sha256Init(MemCh *m);
status Digest_Sha256Update(struct secure *sha, Str *s);
Str *Digest_Sha256Final(MemCh *m);
