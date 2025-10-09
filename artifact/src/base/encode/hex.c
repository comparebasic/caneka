#include <external.h>
#include <caneka.h>

Str *Bytes_ToHexStr(MemCh *m, byte *b, i16 length){
    if(length > STR_MAX*2){
        Error(ErrStream->m, NULL, FUNCNAME, FILENAME, LINENUMBER,
            "Error str length doubled would be beyond a single str and no "
            "length provided to indicate truncation", NULL);
        return NULL;
    }

    Str *s = Str_Make(m, length*2);
    s->type.state |= STRING_ENCODED;
    byte *nb = s->bytes;
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

    s->length = length*2;

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

Str *Str_FromHex(MemCh *m, Str *s){
    Str *n = Str_Make(m, (s->length/2));
    byte *b = s->bytes;
    byte *nb = n->bytes;
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
    n->length = s->length/2;
    return n;
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
