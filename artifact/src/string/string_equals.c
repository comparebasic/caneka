#include <external.h>
#include <caneka.h>

boolean String_PosEqualsBytes(String *a, byte *cstr, int length, word pos_fl){
    DebugStack_Push(a, a->type.of);
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
            DebugStack_Pop();
            return FALSE;
        }
        pos = l - length;
    }else if(String_Length(a) != length){

        if((a->type.state & DEBUG) != 0){
            printf("String_Equals:mismatch@length(%ld/%d)\n", String_Length(a), length);
        }

        DebugStack_Pop();
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
            DebugStack_Pop();
            return FALSE;
        }

        p += tail->length;
        actual += tail->length;
        pos += tail->length-offset;
        tail = String_Next(tail);
    }

    if(tail == NULL && pos == l){
        DebugStack_Pop();
        return TRUE;
    }

    DebugStack_Pop();
    return FALSE;
}

boolean String_EqualsBytes(String *a, byte *cstr){
    return String_PosEqualsBytes(a, cstr, strlen((char *)cstr), STRING_POS_ALL);
}

boolean String_Eq(Abstract *a, void *b){
    String *sa = (String *)a;
    String *sb = (String *)b;
    return String_Equals(sa, sb);
}

boolean String_EqualsPartial(String *a, String *b, word pos){
    if(a == NULL || a->length == 0){
        Fatal("String a is NULL or of 0 length", TYPE_STRING_CHAIN);
    }
    if(b == NULL || b->length == 0){
        Fatal("String b is NULL or of 0 length", TYPE_STRING_CHAIN);
    }
    i64 aLength = String_Length(a);
    i64 bLength = String_Length(b);
    if(pos == STRING_POS_ALL && aLength != bLength){
        if((a->type.state & DEBUG) != 0){
            printf("String_Equals:mismatch@length(%ld/%ld)\n", aLength, bLength);
        }
        return FALSE;
    }else if (bLength > aLength){
        return FALSE;
    }

    i64 remaining = aLength;
    i64 start = 0;
    int length = 0;
    if(pos != STRING_POS_ALL){
        remaining = bLength;
    }
    if(pos == STRING_POS_END){
        start = aLength - bLength;
    }
    String *aTail = a;
    String *bTail = b;
    i64 total = 0;
    while(aTail != NULL && bTail != NULL && remaining > 0){
        if(aTail->length != bTail->length){
            return FALSE;
        }
        if(start <= 0){
            byte *aPtr = aTail->bytes+start;
            byte *bPtr = bTail->bytes+start;
            length = min(remaining, aTail->length-start);
            if(strncmp((char *)aPtr, (char *)bPtr, length) != 0){
                if((a->type.state & DEBUG) != 0){
                    printf("String_Equals:mismatch@[%ld..%ld]\n", total, total + aTail->length);
                }
                return FALSE;
            }
            remaining -= length;
        }else{
            length = aTail->length;
        }
        total += length;
        aTail = String_Next(aTail);
        bTail = String_Next(bTail);
    }

    if(remaining == 0){
        return TRUE;
    }
    
    if((a->type.state & DEBUG) != 0){
        printf("String_Equals:mismatch:end-not-null@[%ld..%ld]", total, total + aTail->length);
    }

    return FALSE;
}

boolean String_Equals(String *a, String *b){
    return String_EqualsPartial(a, b, STRING_POS_ALL);
}

boolean String_StartsWith(String *a, String *b){
    return String_EqualsPartial(a, b, STRING_POS_START);
}

boolean String_EndsWith(String *a, String *b){
    return String_EqualsPartial(a, b, STRING_POS_END);
}
