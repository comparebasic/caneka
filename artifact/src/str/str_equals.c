#include <external.h>
#include <caneka.h>

boolean Str_EqualsStr(Str *a, Str *b){
    if(a == NULL || b == NULL){
       return FALSE; 
    }
    if(a->length != b->length){
        return FALSE;
    }
    i32 length = a->length;
    util *ptrA = (util *)a->bytes;
    util *ptrB = (util *)b->bytes;
    while(length >= sizeof(util)){
        length -= sizeof(util);
        if(*ptrA != *ptrB){
            return FALSE;
        }
        ptrA++;
        ptrB++;
    }
    if(length > 0){
       util shelfA = 0;
       util shelfB = 0;
       memcpy(&shelfA, ptrA, length);
       memcpy(&shelfB, ptrB, length);
       if(shelfA != shelfB){
           return FALSE;
       }
    }
    return TRUE;
}

boolean Str_EqualsStrVec(Str *a, StrVec *b){
    if(a == NULL || b == NULL){
       return FALSE; 
    }
    if(a->length != b->total){
        return FALSE;
    }
    i32 length = a->length;
    util *ptrA = (util *)a->bytes;
    byte *ptrBb;
    byte *end;
    i16 needed = sizeof(util);
    i64 pos = 0;
    util shelfA = 0;
    util shelfB = 0;
    Str *s = NULL;
    Iter_Reset(&b->it);
    i64 remaining = 0;
    while(length > 0 && (Iter_Next(&b->it) & END) == 0){
        pos = 0;
        s = (Str *)Iter_Get(&b->it);
        printf("next S%hd\n", s->length);
        ptrBb = s->bytes+pos;
        end = s->bytes+s->length-1;
        while(ptrBb < end){
            printf("needed: %d\n", needed);
            while(needed > 0 && ptrBb < end){
                remaining = (end+1) - ptrBb;
                word len = min(remaining, needed);
                printf("copying %d\n", len);
                memcpy(((byte *)(&shelfB))+pos, ptrBb, len);
                needed -= len;
                ptrBb += len;
                pos += len;
            }

            write(0, "comparing: '", strlen("comparing: '"));
            write(0, ptrA, 8);
            write(0, "' vs '", strlen("' vs '"));
            write(0, &shelfB, 8);
            write(0, "'\n", 2);
            if(length < sizeof(util)){
                util shelfA = 0;
                memcpy(&shelfA, ptrA, length);
            }
            if(needed > 0 && remaining > 0){
                printf("need:%d pos:%ld\n", needed, pos);
                fflush(stdout);
                write(0, &shelfB, 8);
                break;
            }else if(shelfA != shelfB){
                    return FALSE;
            }else if(*ptrA != shelfB){
                return FALSE;
            }
            shelfB = 0;
            length -= sizeof(util);
            needed = sizeof(util);
            pos = 0;
            ptrA++;
        }
    }

    return TRUE;
}

boolean StrVec_EqualsStr(StrVec *a, Str *b){
    return FALSE;
}

boolean StrVec_EqualsStrVec(StrVec *a, StrVec *b){
    return FALSE;
}
