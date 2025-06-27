#include <external.h>
#include <caneka.h>

static boolean Str_equalsStrPos(Str *a, i32 offsetA, Str *b, i32 offsetB, i32 total){
    if(a == NULL || b == NULL){
       return FALSE; 
    }

    i32 aLength = min(a->length - offsetA, total);
    i32 bLength = min(b->length - offsetB, total);
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
    return Str_equalsStrPos(a, 0, b, 0, a->length);
}

boolean Str_EndMatch(Str *a, Str *b){
    if(b->length > a->length){
        return FALSE;
    }
    word length = a->length - b->length;
    return Str_equalsStrPos(a, length, b, 0, length);
}

boolean Str_StartMatch(Str *a, Str *b, i32 length){
    return Str_equalsStrPos(a, 0, b, 0, length);
}

boolean Str_EqualsStrVec(Str *a, StrVec *b){
    if(a == NULL || b == NULL){
       return FALSE; 
    }
    i32 length = a->length;
    if(a->length != b->total && (a->type.state & DEBUG) == 0){
        goto miss;
    }
    util *ptrA = (util *)a->bytes;
    util *ptrB = NULL;
    Cursor curs;
    Cursor_Setup(&curs, b);
    while(length >= 8 && (StrVec_NextSlot(b, &curs) & END) == 0){
        if(*ptrA != curs.slot){
            goto miss;
        }
        ptrA++;
        ptrB++;
        length -= 8;
    }

    if(length > 0 && (StrVec_NextSlot(b, &curs) & END) == 0){
        util shelfA = 0;
        memcpy(&shelfA, ptrA, length);
        if(shelfA != curs.slot){
            goto miss;
        }
    }

    return TRUE;
miss:
    if((a->type.state & DEBUG) || (b->type.state & DEBUG)){
        i32 pos = a->length - length;
        i16 hlLength = min(a->length - pos, 16);
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(ErrStream->m, a->length),
            (Abstract *)I64_Wrapped(ErrStream->m, b->total),
            (Abstract *)I32_Wrapped(ErrStream->m, pos),
            (Abstract *)Str_Ref(ErrStream->m, a->bytes+pos, hlLength, hlLength, READY),
            (Abstract *)a,
            (Abstract *)b,
            NULL
        };
        Debug("Str_EqualsStrVec Differ ^D.$^d.length vs ^D.$^d.length at $/^E.@^e. & vs &\n", args);
    }
    return FALSE;
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
