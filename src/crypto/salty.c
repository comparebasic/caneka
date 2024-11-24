#include <basictypes_external.h> 
#include <caneka/caneka.h> 

static Typed *phrase_enc = NULL;

Typed *Salty_GetPEnc(MemCtx *m){
    if(phrase_enc == NULL){
        phrase_enc = Alloc(m, sizeof(Typed));
        phrase_enc->type = TYPE_ENC_TYPE;
        SetFlag(phrase_enc, FLAG_SALTY_ENC);
    }

    return phrase_enc;
}

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */
static void Tea_encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static void Tea_decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static String *Salty_process(MemCtx *m, String *key, String *s, cbool_t enc){
    util_t alignement = s->nbytes;
    String *s2 = String_ReAlign(m, s, STRING_DEFAULT_ALIGNMENT);
    String *tail_s = NULL;
    /* loop over the string segments */
    while(tail_s != NULL){
        if(tail_s->nbytes % SALTY_VALUE_SIZE != 0){
            printf("Error: value string not on bounds, is it aligned?\n");
            return NULL;
        }

        tail_s = s2;
        void *vs_ptr = (void *)tail_s->content;
        util_t offset = 0;

        /* one tea value at a time */
        int rounds = tail_s->nbytes / SALTY_VALUE_SIZE;
        for(int i = 0; i < rounds; i++){
            if(key->nbytes % SALTY_KEY_SEGSIZE != 0){
                printf("Error: key string not on bounds, is it aligned?\n");
                return NULL;
            }

            /* one tea-key value at a time */
            int keyRounds = key->nbytes / SALTY_KEY_SEGSIZE;
            void *key_v = (void *)key->content;
            for(int j = 0; j < keyRounds; j++){
                if(enc){
                    Tea_encipher(SALTY_TEA_ROUNDS, (uint32_t *)vs_ptr, (uint32_t *)key_v); 
                }else{
                    Tea_decipher(SALTY_TEA_ROUNDS, (uint32_t *)vs_ptr, (uint32_t *)key_v); 
                }
                key_v += SALTY_KEY_SEGSIZE;
            }
        }

        vs_ptr += SALTY_VALUE_SIZE;
        offset += SALTY_VALUE_SIZE;

        if(offset == tail_s->nbytes){
            tail_s = tail_s->next_s;
        }
    }

    return s2;
}

String *Salty_MakeKey(MemCtx *m, String *s){
    util_t length = SALTY_KEY_MINSIZE;
    if(s->nbytes > length){
        length = ((s->nbytes / SALTY_KEY_SEGSIZE) + 1) * SALTY_KEY_SEGSIZE;
    }

    String *s2 = String_Clone(m, s);
    String_Resize(m, s2, length);
    util_t delta = min(length - s->nbytes, s->nbytes);
    util_t remaining = length - s->nbytes;
    uchar *p = s2->content + s->nbytes;
    while(remaining > 0){
        memcpy(p, s->content, delta);
        p += delta;
        remaining -= delta;
    }
    s2->nbytes = length;

    return s2;
}


String *Salty_Enc(MemCtx *m, String *key, String *s){
    return Salty_process(m, key, s, true);
}

String *Salty_Dec(MemCtx *m, String *key, String *s){
    return Salty_process(m, key, s, false);
}

String *Salty_Hash(MemCtx *m, String *key, String *s){
    return NULL;
}
