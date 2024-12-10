#include <external.h>
#include <caneka.h>

status Sha256_AddString(Sha256 *sha, String *s){
    status r = READY;
    while(s != NULL){
        int ok = Sha256_process(sha, s->bytes, (unsigned long)s->length);
        printf("%d\n", ok);
        printf("Adding: %s\n", String_ToEscaped(DebugM, s)->bytes);
        s = String_Next(s);
        r |= SUCCESS;
    }
    return r;
}

String *Sha256_Get(MemCtx *m, Sha256 *sha){
    byte buff[SHA256_DIGEST_SIZE+1];
    memset(buff, 0, sizeof(buff));

    int ok = Sha256_done(sha, buff);
    printf("%d\n", ok);
    String *ret = String_Init(m, SHA256_DIGEST_SIZE);
    String_AddBytes(m, ret, buff, SHA256_DIGEST_SIZE);
    return ret;
}

String *String_Sha256(MemCtx *m, String *s){
    struct sha256_state _st;
    memset(&_st, 0, sizeof(_st));
    Sha256_init(&_st);
    byte buff[SHA256_DIGEST_SIZE+1];
    memset(buff, 0, sizeof(buff));

    Debug_Print((void *)s, 0, "Sha source: ", 0, TRUE);
    printf("\n");

    while(s != NULL){
        Sha256_process(&_st, s->bytes, (unsigned long)s->length);
        printf("Adding: %s\n", String_ToEscaped(DebugM, s)->bytes);
        s = String_Next(s);
    }

    Sha256_done(&_st, buff);
    String *ret = String_Init(m, SHA256_DIGEST_SIZE);
    String_AddBytes(m, ret, buff, SHA256_DIGEST_SIZE);

    Debug_Print((void *)String_ToHex(m, ret), 0, "Sha256: ", COLOR_RED, TRUE);
    printf("\n");

    return ret;
}

status EcKeyPair_Make(MemCtx *m, String *priv, String *pub){
    return NOOP;
}

String *Sign_Ecdsa(MemCtx *m, String *s, String *priv){
    return NULL;
}

boolean Verify_Ecdsa(MemCtx *m, String *s, String *priv){
    return FALSE;
}

String *Crypto_RandomString(MemCtx *m, int length){
   File file;
   File_Init(&file, String_Make(m, bytes("/dev/random")), NULL, NULL);
   file.abs = file.path;
   File_Read(m, &file, NULL, 0, length);
   return file.data;
}
