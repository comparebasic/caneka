#define CRYPTO_SECRETKEYBYTES 64
#define CRYPTO_PUBLICKEYBYTES 32
#define CRYPTO_BYTES 64

#include <nacl/crypto_sign.h>
#include <nacl/crypto_hash_sha512.h>
#include <nacl/randombytes.h>
#include <nacl/crypto_verify_32.h>

#include <external.h>
#include "module.h" 

#include "ge25519.h"

int sign_keypair_sha256( unsigned char *pk, unsigned char *sk, unsigned char *digest){
  sc25519 scsk;
  ge25519 gepk;

  memcpy(sk, digest, 32);
  sk[0] &= 248;
  sk[31] &= 127;
  sk[31] |= 64;

  sc25519_from32bytes(&scsk,sk);
  
  ge25519_scalarmult_base(&gepk, &scsk);
  ge25519_pack(pk, &gepk);
  return 0;
}
