#include <external.h>
#include "base_module.h"

Str *Str_ToUpper(MemCh *m, Str *s){
    Str *upper = s;
    if(s->type.state & STRING_COPY){
        upper = Str_Clone(m, s);
    }
    byte *ptr = upper->bytes;
    byte *end = upper->bytes+(upper->length-1);
    i32 delta = 'a' - 'A';
    while(ptr <= end){
        byte b = *ptr;
        if(b >= 'a' && b <= 'z'){
            *ptr = b-delta;
        }
        ptr++;
    }
    return upper;
}

Str *Str_ToLowerFiltered(MemCh *m, Str *s, Str *filter, byte replace){
    Str *lower = s;
    if(s->type.state & STRING_COPY){
        lower = Str_Clone(m, s);
    }
    byte *ptr = lower->bytes;
    byte *end = lower->bytes+(lower->length-1);
    i32 delta = 'a' - 'A';
    while(ptr <= end){
        byte b = *ptr;
        if(b >= 'A' && b <= 'Z'){
            *ptr = b+delta;
        }else{
            for(i32 i = 0; i < filter->length; i += 2){
                if(b >= filter->bytes[i] && b <= filter->bytes[i+1]){
                    *ptr = replace;
                }
            }
        }
        ptr++;
    }
    return lower;
}


Str *Str_ToLower(MemCh *m, Str *s){
    Str *lower = s;
    if(s->type.state & STRING_COPY){
        lower = Str_Clone(m, s);
    }
    byte *ptr = lower->bytes;
    byte *end = lower->bytes+(lower->length-1);
    i32 delta = 'a' - 'A';
    while(ptr <= end){
        byte b = *ptr;
        if(b >= 'A' && b <= 'Z'){
            *ptr = b+delta;
        }
        ptr++;
    }
    return lower;
}

Str *Str_ToTitle(MemCh *m, Str *s){
    Str *title = s;
    if(s->type.state & STRING_COPY){
        title = Str_Clone(m, s);
    }
    byte *ptr = title->bytes;
    byte *end = title->bytes+(title->length-1);
    i32 delta = 'a' - 'A';
    while(ptr <= end){
        byte b = *ptr;
        if(ptr == title->bytes || 
                (!(*(ptr-1) >= 'A' && *(ptr-1) <= 'Z')) &&
                (!(*(ptr-1) >= 'a' && *(ptr-1) <= 'z')) &&
                (b >= 'a' && b <= 'z')
            ){
            *ptr = b-delta;
        }
        ptr++;
    }
    return title;
}

status Str_AddChain(MemCh *m, Str *s, void *args[]){
    status r = READY;
    Abstract **ptr = (Abstract **)args;
    while(*ptr != NULL && (s->type.state & ERROR) == 0){
        Abstract *a = (Abstract *)*ptr;
        if(a->type.of == TYPE_STRVEC){
            Str_AddVec(s, (StrVec *)a);
            r |= SUCCESS;
        }else if(a->type.of == TYPE_STR){
            Str_Add(s, ((Str *)a)->bytes, ((Str *)a)->length); 
            r |= SUCCESS;
        }
        ptr++;
    }
    
    if(r == READY){
        r |= NOOP;
    }

    return r|s->type.state;
}

Str *Str_MemCount(MemCh *m, i64 mem) {
    Str *s = Str_Make(m, STR_DEFAULT);
    Str_AddMemCount(s, mem);
    return s;
}

i64 Str_Trunc(Str *s, i64 amount){
    if(s->type.state & STRING_CONST){
        void *args[] = {s ,NULL};
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Str_Trunc cannot modify a const char * '$'", args);
    }
    if(amount == 0){
        return 0;
    }else if(amount < 0){
        amount = labs(amount);
        if(s->length < amount){
            return 0;
        }
        byte *ptr = s->bytes+(s->length-amount);
        memset(ptr, 0, amount);
        s->length -= amount;
    }else if(amount > 0){
        if(amount > s->length){
            return 0;
        }
        i64 removed = s->length - amount;
        s->length = amount;
        memset(s->bytes+s->length, 0, removed);
    }
    return amount;
}

Str *Str_BuffFromCstr(MemCh *m, char *cstr){
    return Str_Ref(m, (byte *)cstr, strlen(cstr), STR_DEFAULT, STRING_COPY);
}

Str *Str_Prefixed(MemCh *m, Str *s, Str *prefix){
    if(s->length + prefix->length < s->alloc){
        memmove(s->bytes+prefix->length, s->bytes, s->length);
        memcpy(s->bytes, prefix->bytes, prefix->length);
        return s;
    }else{
        Str *s2 = Str_Make(m, s->length+prefix->length+1);
        memcpy(s2->bytes, prefix->bytes, prefix->length);
        memcpy(s2->bytes+prefix->length, s->bytes, s->length);
        return s2;
    }
}

i64 Str_AddMemCount(Str *s, i64 mem) {
    if(mem == 0){
       return Str_AddCstr(s, "0b"); 
    }
    char *abbrev = "bkmg";
    i64 total = 0;
    i64 value = 0;
    boolean exactly = FALSE;
    for(int i = 0; i < 4; i++){
        value = mem & 1023;
        if(value){
           if(s->length > 0){
               total += Str_AddCstr(s, " ");
           }
           total += Str_AddI64(s, value); 
           total += Str_Add(s, (byte *)abbrev+i, 1); 
        }
        mem = mem >> 10;
    }

    return total;
}
