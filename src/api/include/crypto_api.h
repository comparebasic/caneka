#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#define DIGEST_SIZE 32
#define KEY_SIZE 2048
#define SIG_FOOTER_SIZE 64
#define SIGNATURE_SIZE 96

enum crypto_range {
    _TYPE_CRYPTO_START = _TYPE_ALGO_END,
    TYPE_ECKEY,
    TYPE_ECKEY_PUB,
    TYPE_TLS_CTX,
    TYPE_TLS_INFO,
    TYPE_TLS_CAPSULE,
    _TYPE_CRYPTO_END,
};

typedef struct crypto_tls_ctx {
    Type type;
    void *tlsCtx;
} TlsCtx;

typedef struct crypto_tls_info {
    Type type;
    i32 fd;
    void *tls;
    TlsCtx *ctx;
} TlsInfo;

typedef byte digest[DIGEST_SIZE];

void Tls_Init(MemCh *m);
TlsCtx *TlsCtx_Make(MemCh *m, StrVec *cert, StrVec *key);
void TlsCtx_Destroy(MemCh *m, TlsCtx *ctx);
TlsInfo *TlsInfo_Make(MemCh *m, i32 fd, TlsCtx *ctx);

status Str_ToSha256(MemCh *m, Str *s, digest *hash);
status StrVec_ToSha256(MemCh *m, StrVec *v, digest *hash);

status StrVec_SaltedDigest(MemCh *m,
    StrVec *v, Str *salt, digest *hash, util nonce);

status SignPair_Make(MemCh *m, Single *public, Single *secret);
status SignPair_FromPhrase(MemCh *m, Single *public, Single *secret, StrVec *phrase);
Str *SignPair_Sign(MemCh *m, StrVec *content, Single *secret);
status SignPair_Verify(MemCh *m, StrVec *content, Str *sig, Single *public);
status BoxPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase);
status BoxPair_Enc(MemCh *m, Str *secret, StrVec *content);
status BoxPair_Dec(MemCh *m, Str *public, StrVec *content);
status Str_HmacEnc(MemCh *m, Str *s, Str *hmac);
status Str_HmacDec(MemCh *m, Str *s, Str *hmac);
Str *Str_DigestAlloc(MemCh *m);
status SignPair_PrivateFromPem(Buff *bf, Single *secret);
status SignPair_PublicFromPem(Buff *bf, Single *public);
status SignPair_PrivateToPem(Buff *bf, Single *secret);
status SignPair_PublicToPem(Buff *bf, Single *public);

status Crypto_Init(MemCh *m);
#endif
