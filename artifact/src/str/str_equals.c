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
    util *ptrB = NULL;
    StrVecCursor *curs = &_strVecCurs;
    StrVecCurs_Setup(b, curs);
    while(length >= sizeof(util) && (StrVec_NextSlot(b, curs) & END) == 0){
        write(0, "current: '", strlen("current: '"));
        write(0, &curs->slot, 8);
        write(0, "' vs '", strlen("' vs '"));
        write(0, ptrA, 8);
        write(0, "'\n", 2);
        if(*ptrA != curs->slot){
            return FALSE;
        }
        ptrA++;
        ptrB++;
        length -= sizeof(util);
    }

    while(length > 0 && (StrVec_NextSlot(b, curs) & END) == 0){
       util shelfA = 0;
       memcpy(&shelfA, ptrA, length);
        write(0, "current: '", strlen("current: '"));
        write(0, &curs->slot, 8);
        write(0, "' vs '", strlen("' vs '"));
        write(0, &shelfA, 8);
        write(0, "'\n", 2);
       if(shelfA != curs->slot){
           return FALSE;
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
