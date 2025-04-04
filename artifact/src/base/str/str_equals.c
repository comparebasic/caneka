#include <external.h>
#include <caneka.h>

static boolean Str_equalsStrPos(Str *a, i32 offsetA, Str *b, i32 offsetB){
    if(a == NULL || b == NULL){
       return FALSE; 
    }

    i32 aLength = a->length - offsetA;
    i32 bLength = b->length - offsetB;
    if(aLength < 0 || bLength < 0){
        return FALSE;
    }
    if(aLength != bLength){
        return FALSE;
    }
    i32 length = aLength;
    util *ptrA = (util *)(a->bytes+offsetA);
    util *ptrB = (util *)(b->bytes+offsetB);
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

boolean Str_EqualsStr(Str *a, Str *b){
    return Str_equalsStrPos(a, 0, b, 0);
}

boolean Str_EndMatch(Str *a, Str *b){
    if(b->length > a->length){
        return FALSE;
    }
    return Str_equalsStrPos(a, a->length - b->length, b, 0);
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
    Cursor curs;
    Cursor_Setup(&curs, b);
    while(length >= 8 && (StrVec_NextSlot(b, &curs) & END) == 0){
        if(*ptrA != curs.slot){
            return FALSE;
        }
        ptrA++;
        ptrB++;
        length -= 8;
    }

    if(length > 0 && (StrVec_NextSlot(b, &curs) & END) == 0){
        util shelfA = 0;
        memcpy(&shelfA, ptrA, length);
        if(shelfA != curs.slot){
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
    Cursor cursA;
    Cursor_Setup(&cursA, a);
    Cursor cursB;
    Cursor_Setup(&cursB, b);
    while(length > 8 && (StrVec_NextSlot(a, &cursA) & END) == 0 && 
            (StrVec_NextSlot(b, &cursB) & END) == 0){
        if(cursA.slot != cursB.slot){
            return FALSE;
        }
        length -= 8;
    }

    if(length > 0 &&
            (StrVec_NextSlot(a, &cursA) & END) == 0 && 
            (StrVec_NextSlot(b, &cursB) & END) == 0){
        if(cursA.slot != cursB.slot){
            return FALSE;
        }
    }

    return TRUE;
}
