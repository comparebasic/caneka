#ifndef CNK_NACL_H
#define CNK_NACL_H
#include <crypto_hash_sha256.h>
#include <crypto_hash_sha256.h>
#include <crypto_auth_hmacsha256.h>
#include <crypto_sign.h>
#include <crypto_box.h>

#ifndef crypto_uint16_h
#define crypto_uint16_h
typedef unsigned short crypto_uint16;
#endif
#ifndef crypto_uint32_h
#define crypto_uint32_h
typedef unsigned int crypto_uint32;
#endif
#ifndef crypto_uint64_h
#define crypto_uint64_h
typedef unsigned long long crypto_uint64;
#endif

struct sha256_ctx;

typedef struct sha256_ctx {
  int type;
  unsigned long long total;
  unsigned char h[32];
} Sha256Ctx;

int sha256_start(struct sha256_ctx *ctx);
int sha256_update(struct sha256_ctx *ctx,
    const unsigned char *in,unsigned long long inlen);
int sha256_finalize(struct sha256_ctx *ctx,
    unsigned char *out,const unsigned char *in,unsigned long long inlen);
int sign_keypair_sha256(unsigned char *pk, unsigned char *sk, unsigned char *digest);
#endif
