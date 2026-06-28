#include <external.h>
#include "base_module.h"

void Bytes_ToHexOntoStr(MemCh *m, byte *b, byte *ptr, Str *s, i16 length){
    if(Str_Remaining(s, ptr) < length*2){
        void *ar[] = {
            I16_Wrapped(m, length*2),
            I16_Wrapped(m, Str_Remaining(s, ptr)),
            s,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Not enough room left on string needed $, have $, on &", ar);
    }

    s->type.state |= STRING_ENCODED;
    byte *nb = ptr;
    for(i32 i = 0; i < length; i++){
        byte c;
        c = b[i] >> 4;
        nb[i*2] =  c  < 10 ?
            c + '0' : 
            (c - 10) + 'a';
        c = b[i] & 15;
        nb[i*2+1] =  c  < 10 ?
            c + '0' : 
            (c - 10) + 'a';
    }

    s->length += length*2;
}

Str *Bytes_ToHexStr(MemCh *m, byte *b, i16 length){
    if(length > STR_MAX*2){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error str length doubled would be beyond a single str and no "
            "length provided to indicate truncation", NULL);
        return NULL;
    }

    Str *s = Str_Make(m, length*2);
    s->type.state |= STRING_ENCODED;
    Bytes_ToHexOntoStr(m, b, s->bytes, s, length);

    return s;
}

Str *Str_ToHex(MemCh *m, Str *s){
    return Bytes_ToHexStr(m, s->bytes, s->length);
}

StrVec *StrVec_ToHex(MemCh *m, StrVec *v){
    StrVec *n = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, v->p);
    i16 halfs = STR_MAX/2;
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);

        i16 length = min(s->length, halfs);
        StrVec_Add(n, Bytes_ToHexStr(m, s->bytes, length));

        if(s->length*2 > STR_MAX){
            length = s->length - halfs;
            StrVec_Add(n, Bytes_ToHexStr(m, s->bytes+halfs, length));
        }
    }

    return n;
}

util Util_FromHex(MemCh *m, Str *s){
    if(s->length != sizeof(util)*2){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Wrong size str of bytes to decode into a util", NULL);
        return 0;
    }
    util u = 0;
    Str *ref = Str_Ref(m, (byte *)&u, sizeof(util), sizeof(util), ZERO);
    Bytes_FromHex(m, ref, s->bytes);
    return u;
}

status Bytes_FromHex(MemCh *m, Str *s, byte *nb){
    byte *b = s->bytes;
    i32 bi;
    for(i32 i = 0; i < s->length; i += 2){
        bi = i/2;
        byte c = b[i];
        nb[bi] = c >= 'a' ?  
            ((c - 'a') + 10) << 4:
            (c - '0') << 4;
        c = b[i+1];
        nb[bi] |= c >= 'a' ?  
            c - 'a' + 10:
            c - '0';
    }
    return ZERO;
}

Str *Str_FromHex(MemCh *m, Str *s){
    Str *n = Str_Make(m, (s->length/2));
    Bytes_FromHex(m, s, n->bytes);
    n->length = s->length/2;
    return n;
}

Str *Str_FromHexFiltered(MemCh *m, Str *s){
    for(i32 i = 0; i < s->length; i++){
        byte c = s->bytes[i];
        if(c >= 'A' && c <= 'F'){
            s->bytes[i] = c + 62;
        }else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')){
            continue;
        }else{
            Str *blank = Str_MakeBlank(m);
            blank->type.state |= ERROR;
            return blank;
        }
    }
    
    return Str_FromHex(m, s);
}

status Raw_FromHex(MemCh *m, Str *s, void *b, i64 sz){
    if(sz != s->length/2){
        return ERROR;
    }
    Bytes_FromHex(m, s, (byte *)b);
    return ZERO;
}

StrVec *StrVec_FromHex(MemCh *m, StrVec *v){
    StrVec *n = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        StrVec_Add(n, Str_FromHex(m, s));
    }

    return n;
}
