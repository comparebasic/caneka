#include <external.h>
#include <caneka.h>

Str *Str_FromI64(MemCh *m, i64 i){
    Str *s = Str_Make(m, MAX_BASE10);
    Str_AddI64(s, i);
    return s;
}

i64 Str_I64OnBytes(byte **_b, i64 i){
    byte *end = (*_b)+(MAX_BASE10-1);
    byte *b = end;

    i64 base = 10;
    i64 val;
    boolean negative = i < 0;
    if(negative){
        i = labs(i);
    }
    *b = '0';
    while(i > 0){
        val = i % base;
        *(b--) = '0'+val;
        i -= val;
        i /= base;
    }

    if(negative){
        *(b--) = '-';
    }else if(b == end){
        *(b--) = '0';
    }

    *_b = b+1;
    return end - b;
}

i64 Str_AddIByte(Str *s, byte i){
    byte _b[MAX_BASE10];
    if(s->alloc < s->length+4){
        s->type.state |= ERROR;
        return 0;

    }

    byte *b = _b;
    i64 n = i;
    i64 length = Str_I64OnBytes(&b, n);
    return Str_Add(s, b, length);
}


i64 Str_AddI64(Str *s, i64 i){
    byte _b[MAX_BASE10];
    if(s->alloc < s->length+MAX_BASE10){
        s->type.state |= ERROR;
        return 0;

    }

    byte *b = _b;
    i64 length = Str_I64OnBytes(&b, i);
    return Str_Add(s, b, length);
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
