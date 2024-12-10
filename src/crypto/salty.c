#include <external.h> 
#include <caneka.h> 

static void Tea_encipher(unsigned int rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static void Tea_decipher(unsigned int rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*rounds;
    for (i=0; i < rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static status Salty_process(MemCtx *m, String *key, String *s, Salty_Proc proc){
    if(key->length % SALTY_KEY_SEGSIZE != 0){
        printf("Error: key string not on bounds, is it aligned?\n");
        return ERROR;
    }

    while(s != NULL){
        void *ptr = (void *)s->bytes;
        int offset = 0;

        int rounds = s->length / SALTY_VALUE_SIZE;
        if(s->length % SALTY_VALUE_SIZE != 0){
            rounds++;
            s->length = rounds * SALTY_VALUE_SIZE;
        }
        printf("rounds: %d/%ld = %d\n", s->length, SALTY_VALUE_SIZE, rounds);
        for(int i = 0; i < rounds; i++){
            printf("offset %ld\n", (byte *)ptr - s->bytes);
            int keyRounds = key->length / SALTY_KEY_SEGSIZE;
            void *kPtr = (void *)key->bytes;
            if(proc == Tea_decipher){
                kPtr += (key->length-SALTY_KEY_SEGSIZE);
            }
            for(int j = 0; j < keyRounds; j++){
                proc(SALTY_TEA_ROUNDS, (uint32_t *)ptr, (uint32_t *)kPtr); 
                if(proc == Tea_decipher){
                    kPtr -= SALTY_KEY_SEGSIZE;
                }else{
                    kPtr += SALTY_KEY_SEGSIZE;
                }
            }
            ptr += SALTY_VALUE_SIZE;
            offset += SALTY_VALUE_SIZE;
        }
        s = String_Next(s);
    }

    return SUCCESS;
}

String *Salty_MakeKey(MemCtx *m, String *s){
    i64 length = SALTY_KEY_MINSIZE;
    if(s->length < length || (s->length % SALTY_KEY_SEGSIZE) != 0){
        length = ((s->length / SALTY_KEY_SEGSIZE)+1) * SALTY_KEY_SEGSIZE;
    }

    String *key = String_Clone(m, s);

    if(key->length < length){
        String_AddBytes(m, key, key->bytes, length - key->length);
    }

    return key;
}

status Salty_Enc(MemCtx *m, String *key, String *s){
    return Salty_process(m, key, s, Tea_encipher);
}

status Salty_Dec(MemCtx *m, String *key, String *s){
    return Salty_process(m, key, s, Tea_decipher);
}
