#include <external.h>
#include <caneka.h>


static char *cryptoTypeStrings[] = {
    "_TYPE_CRYPTO_START",
    "TYPE_ECKEY",
    "TYPE_ECKEY_PUB",
    "TYPE_TLS_CTX",
    "TYPE_TLS_INFO",
    "TYPE_TLS_CAPSULE",
    "_TYPE_CRYPTO_END",
    NULL
};

status CryptoTypeStrings_Init(MemCh *m){
    status r = READY;
    r |=  Lookup_Add(m, TypeStringRanges, _TYPE_CRYPTO_START, (void *)cryptoTypeStrings);
    return r;
}

status Crypto_Init(MemCh *m){
    status r = READY;
    Tls_Init(m);
    r |= CryptoTypeStrings_Init(m);
    r |= OpenSsl_ExtFreeInit(m);;
    r |= Tls_ToSInit(m, ToStreamLookup);
    return r;
}
