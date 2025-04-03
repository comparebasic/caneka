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
        printf("Length mismatch %ld vs %ld\n", a->length, b->total);
        return FALSE;
    }
    i32 length = a->length;
    util *ptrA = (util *)a->bytes;
    util *ptrB = NULL;
    Cursor curs;
    Cursor_Setup(&curs, b);
    while(length >= 8 && (StrVec_NextSlot(b, &curs) & END) == 0){
        if(b->type.state & DEBUG){
            fflush(stdout);
            char *x = (char *)ptrA;
            printf("\n");
            for(i32 i = 0; i < 8; i++){
                printf("%d,",  (i32)*(x+i), (char)*(x+i));
            }
            x = (char *)&curs.slot;
            printf("\n");
            for(i32 i = 0; i < 8; i++){
                printf("%d,",  (i32)*(x+i), (char)*(x+i));
            }
            printf("\n");
        }
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
            printf("mismatch II\n");
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
