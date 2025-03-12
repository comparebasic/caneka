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
    while(length >= 8 && (StrVec_NextSlot(b, curs) & END) == 0){
        if(*ptrA != curs->slot){
            return FALSE;
        }
        ptrA++;
        ptrB++;
        length -= 8;
    }

    if(length > 0 && (StrVec_NextSlot(b, curs) & END) == 0){
        util shelfA = 0;
        memcpy(&shelfA, ptrA, length);
        if(shelfA != curs->slot){
            return FALSE;
        }
    }

    return TRUE;
}

boolean StrVec_EqualsStrVec(StrVec *a, StrVec *b){
    if(a == NULL || b == NULL){
       return FALSE; 
    }
    if(a->total != b->total){
        return FALSE;
    }
    i32 length = a->total;
    StrVecCursor *cursA = &_strVecCurs;
    StrVecCurs_Setup(a, cursA);
    StrVecCursor *cursB = &_strVecCursB;
    StrVecCurs_Setup(b, cursB);
    while(length > 8 && (StrVec_NextSlot(a, cursA) & END) == 0 && 
            (StrVec_NextSlot(b, cursB) & END) == 0){
        if(cursA->slot != cursB->slot){
            return FALSE;
        }
        length -= 8;
    }

    if(length > 0 &&
            (StrVec_NextSlot(a, cursA) & END) == 0 && 
            (StrVec_NextSlot(b, cursB) & END) == 0){
        if(cursA->slot != cursB->slot){
            return FALSE;
        }
    }

    return TRUE;
}
