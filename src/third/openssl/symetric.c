#include <external.h>
#include <caneka.h>

static Str *getIv(MemCh *m, Str *key){
    int half = key->length / 2;
    util a = Hash_Bytes(key->bytes, half);
    util b = Hash_Bytes(key->bytes+half, half);
    Str *s = Str_Make(m, sizeof(util)*2);
    Str_Add(s, (byte *)&a, sizeof(a));
    Str_Add(s, (byte *)&b, sizeof(b));
    return s;
}

StrVec *Symetric_Enc(MemCh *m, Str *key, StrVec *v){
    DebugStack_Push(NULL, 0);
    StrVec *ret = StrVec_Make(m);
    /*
    if((key->type.state & STRING_BINARY) == 0 || key->length != 32){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "requires 32 byte random binary string to perform encryption");
    }

    Str *iv = getIv(m, key);
    Str *ret = Str_Make(m, STR_DEFAULT);
    ret->type.state |= STRING_BINARY;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to create cipher ctx");
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key->bytes, iv->bytes)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to init encryption");
        DebugStack_Pop();
        return NULL;
    }

    byte *b;
    byte b16[16];
    byte c16[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int len;
    int cLen;
    int bLen = sizeof(b16);;
    Iter it;
    Iter_Init(&it, s);
    while((IterStr_Next(&it) & (END|ERROR)) == 0){
        b = (byte *) it.value; 
        if(it.type.state & FLAG_ITER_LAST){
            bLen = Cursor_GetPartial(&(it.cursor), sizeof(b16)); 
        }
        if(1 != EVP_EncryptUpdate(ctx, 
                c16, &len, b, bLen)){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to encrypt");
            DebugStack_Pop();
            return NULL;
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            Str_Add(ret, c16, len);
        }
    }

    cLen = len;
    if(1 != EVP_EncryptFinal_ex(ctx, c16+len, &len)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to finalize encrypt");
        DebugStack_Pop();
        return NULL;
    }
    cLen += len;

    Str_Add(ret, c16, cLen);
    EVP_CIPHER_CTX_free(ctx);

    */
    DebugStack_Pop();
    return ret;
}

StrVec *Symetric_Dec(MemCh *m, Str *key, StrVec *v){
    DebugStack_Push(NULL, 0);
    StrVec *ret = StrVec_Make(m);
    /*
    if((key->type.state & FLAG_STRING_BINARY) == 0 || key->length != 32){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "requires 32 byte random binary string to perform encryption");
        return NULL;
    }

    Str *iv = getIv(m, key);
    Str *ret = Str_Make(m, STR_DEFAULT);

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to create cipher ctx");
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key->bytes, iv->bytes)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to init decryption");
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
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to decrypt");
            DebugStack_Pop();
            return NULL;
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            Str_Add(ret, c16, len);
        }
    }

    cLen = len;
    if(1 != EVP_DecryptFinal_ex(ctx, c16+len, &len)){
        ERR_print_errors_fp(stderr);
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unable to finalize decrypt");
        DebugStack_Pop();
        return NULL;
    }
    cLen += len;

    Str_Add(ret, c16, cLen);
    EVP_CIPHER_CTX_free(ctx);

    */
    DebugStack_Pop();
    return ret;
}
