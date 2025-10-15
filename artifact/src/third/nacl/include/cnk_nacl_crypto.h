#ifndef CNK_NACL_H
#define CNK_NACL_H
#include <nacl/crypto_hash_sha256.h>
#include <nacl/crypto_hash_sha256.h>
#include <nacl/crypto_auth_hmacsha256.h>
#include <nacl/crypto_sign.h>
#include <nacl/crypto_box.h>

#define DIGEST_SIZE 32
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

#endif
