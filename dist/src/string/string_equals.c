#include <external.h>
#include <caneka.h>

boolean String_PosEqualsBytes(String *a, byte *cstr, int length, word pos_fl){
    /* TODO: test this across different string types */
    int pos = 0;
    int actual = 0;
    int offset = 0;
    int match_length = length;
    int l = match_length;
    String *tail = a;
    if(pos_fl == STRING_POS_END){
        l = String_Length(a);
        if(length > l){
            return FALSE;
        }
        pos = l - length;
    }else if(String_Length(a) != length){

        if((a->type.state & DEBUG) != 0){
            printf("String_Equals:mismatch@length(%ld/%d)\n", String_Length(a), length);
        }

        return FALSE;
    }
    byte *p = cstr;
    
    while(tail != NULL && (actual+tail->length) <= l){
        if(actual < pos-tail->length){
            actual += tail->length;
            tail = String_Next(tail);
            continue;
        }

        if(pos > actual){
            offset = pos - actual;
        }else{
            offset = 0;
        }

        if(strncmp((char *)tail->bytes+offset, (char *)p, tail->length-offset) != 0){
            if((a->type.state & DEBUG) != 0){
                
            }
            return FALSE;
        }

        p += tail->length;
        actual += tail->length;
        pos += tail->length-offset;
        tail = String_Next(tail);
    }

    if(tail == NULL && pos == l){
        return TRUE;
    }

    return FALSE;
}

boolean String_EqualsBytes(String *a, byte *cstr){
    return String_PosEqualsBytes(a, cstr, strlen((char *)cstr), STRING_POS_ALL);
}

boolean String_Eq(Abstract *a, void *b){
    if(a->type.of != TYPE_STRING_CHAIN && a->type.of != TYPE_STRING_FIXED
        || ((Abstract *)b)->type.of != TYPE_STRING_CHAIN && ((Abstract *)b)->type.of != TYPE_STRING_FIXED
    ){
       Fatal("", TYPE_STRING_CHAIN); 
    }
    String *sa = (String *)a;
    String *sb = (String *)b;
    return String_Equals(sa, sb);
}

boolean String_Equals(String *a, String *b){
    if(a == NULL){
        Fatal("String a is NULL", TYPE_STRING_CHAIN);
    }
    if(b == NULL){
        Fatal("String b is NULL", TYPE_STRING_CHAIN);
    }
    i64 aLength = String_Length(a);
    i64 bLength = String_Length(b);
    if(aLength != bLength){
        if((a->type.state & DEBUG) != 0){
            printf("String_Equals:mismatch@length(%ld/%ld)\n", aLength, bLength);
        }
        return FALSE;
    }
    String *aTail = a;
    String *bTail = b;
    i64 total = 0;
    while(aTail != NULL && bTail != NULL){
        if(aTail->length != bTail->length){
            return FALSE;
        }
        if(strncmp((char *)aTail->bytes, (char *)bTail->bytes, aTail->length) != 0){
            if((a->type.state & DEBUG) != 0){
                printf("String_Equals:mismatch@[%ld..%ld]\n", total, total + aTail->length);
            }
            return FALSE;
        }
        total += aTail->length;
        if(aTail->type.of == TYPE_STRING_CHAIN){
            aTail = aTail->next;
        }else{
            aTail = NULL;
        }

        if(bTail->type.of == TYPE_STRING_CHAIN){
            bTail = bTail->next;
        }else{
            bTail = NULL;
        }
    }

    if(aTail == NULL && bTail == NULL){
        return TRUE;
    }
    
    if((a->type.state & DEBUG) != 0){
        printf("String_Equals:mismatch:end-not-null@[%ld..%ld]", total, total + aTail->length);
    }

    return FALSE;
}
