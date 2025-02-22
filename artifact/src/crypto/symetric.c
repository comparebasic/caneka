#include <external.h>
#include <caneka.h>

static String *getIv(MemCtx *m, String *key){
    int half = key->length / 2;
    util a = Hash_Bytes(key->bytes, half);
    util b = Hash_Bytes(key->bytes+half, half);
    String *s = String_Init(m, sizeof(util)*2);
    String_AddBytes(m, s, (byte *)&a, sizeof(a));
    String_AddBytes(m, s, (byte *)&b, sizeof(b));
    return s;
}

String *Symetric_Enc(MemCtx *m, String *key, String *s){
    DebugStack_Push(NULL, 0);
    if((key->type.state & FLAG_STRING_BINARY) == 0 || key->length != 32){
        Fatal("requires 32 byte random binary string to perform encryption", TYPE_STRING);
    }

    String *iv = getIv(m, key);
    String *ret = String_Init(m, STRING_EXTEND);
    ret->type.state |= FLAG_STRING_BINARY;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        Fatal("Unable to create cipher ctx", 0);
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key->bytes, iv->bytes)){
        Fatal("Unable to init encryption", 0);
        DebugStack_Pop();
        return NULL;
    }

    byte *b;
    byte b16[16];
    byte c16[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int len;
    int cLen;
    int bLen = sizeof(b16);;
    IterStr it;
    IterStr_Init(&it, s, sizeof(b16), b16);
    while((IterStr_Next(&it) & (END|ERROR)) == 0){
        b = (byte *) IterStr_Get(&it); 
        if(it.type.state & FLAG_ITER_LAST){
            bLen = Cursor_GetPartial(&(it.cursor), sizeof(b16)); 
        }
        if(1 != EVP_EncryptUpdate(ctx, 
                c16, &len, b, bLen)){
            Fatal("Unable to encrypt", 0);
            DebugStack_Pop();
            return NULL;
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            String_AddBytes(m, ret, c16, len);
        }
    }

    cLen = len;
    if(1 != EVP_EncryptFinal_ex(ctx, c16+len, &len)){
        Fatal("Unable to finalize encrypt", 0);
        DebugStack_Pop();
        return NULL;
    }
    cLen += len;

    String_AddBytes(m, ret, c16, cLen);
    EVP_CIPHER_CTX_free(ctx);

    DebugStack_Pop();
    return ret;
}

String *Symetric_Dec(MemCtx *m, String *key, String *s){
    DebugStack_Push(NULL, 0);
    if((key->type.state & FLAG_STRING_BINARY) == 0 || key->length != 32){
        Fatal("requires 32 byte random binary string to perform encryption", TYPE_STRING);
        return NULL;
    }

    String *iv = getIv(m, key);
    String *ret = String_Init(m, STRING_EXTEND);

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        Fatal("Unable to create cipher ctx", 0);
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key->bytes, iv->bytes)){
        Fatal("Unable to init decryption", 0);
        DebugStack_Pop();
        return NULL;
    }

    byte *b;
    byte b16[16];
    byte c16[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int len;
    int cLen;
    int bLen = sizeof(b16);
    IterStr it;
    IterStr_Init(&it, s, sizeof(b16), b16);
    while((IterStr_Next(&it) & (END|ERROR)) == 0){
        b = (byte *) IterStr_Get(&it); 
        if(it.type.state & FLAG_ITER_LAST){
            bLen = Cursor_GetPartial(&(it.cursor), sizeof(b16)); 
        }
        if(1 != EVP_DecryptUpdate(ctx,
                c16, &len, b, bLen)){
            Fatal("Unable to decrypt", 0);
            DebugStack_Pop();
            return NULL;
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            String_AddBytes(m, ret, c16, len);
        }
    }

    cLen = len;
    if(1 != EVP_DecryptFinal_ex(ctx, c16+len, &len)){
        ERR_print_errors_fp(stderr);
        Fatal("Unable to finalize decrypt", 0);
        DebugStack_Pop();
        return NULL;
    }
    cLen += len;

    String_AddBytes(m, ret, c16, cLen);
    EVP_CIPHER_CTX_free(ctx);

    DebugStack_Pop();
    return ret;
}
