/* Base.sequence.Parity
 *
 * Summary hash of any byte content to quickly identify if it is not a match 
 * or warrants futher comparison
 *
 * Bytes are added together according to the size, HalfParity adds 4 bytes, Parity adds 8.
 *
 * At the end the first smallest byte is set to the smallest byte of the content length.
 * This is intended to give a first take at regularly changing variable byte-length content.
 *
 */
#include <external.h>
#include "base_module.h"

boolean HalfParity_Compare(quad par, StrVec *v){
    if((v->total & 7) != (par & 7)){
        return FALSE;
    }else{
        return HalfParity_FromVec(v) == par;
    }
}

quad HalfParity_From(Str *s){
    quad parity = 0;
    quad slot = 0;
    word pos = 0;
    quad size = sizeof(quad);
    quad mod = size-1;
    word remaining = s->length;
    byte *ptr = s->bytes;

    while(remaining >= size){
        memcpy(((byte *)&slot), ptr, size);
        parity += slot;
        remaining -= size;
        ptr += size;
    }

    word tail = remaining & mod;
    if(tail){
        slot = 0;
        memcpy(&slot, ptr, tail);
        parity += slot;
    }

    parity &= ~7;
    parity |= (s->length & 7);

    return parity;
}

quad HalfParity_FromVec(StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    quad parity = 0;
    quad slot = 0;
    word pos = 0;
    quad size = sizeof(quad);
    quad mod = size-1;

    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        word remaining = s->length;

        byte *ptr = s->bytes;
        if(pos > 0){
            word copy = min(remaining, size-pos);
            byte *sptr = (byte *)&slot;
            memcpy(sptr+pos, ptr, copy);
            remaining -= copy;
            if(remaining == 0 && pos < size){
                pos += copy;
                continue;
            }
            parity += slot;
            pos = 0;
            ptr += copy;
        }

        word tail = remaining & mod;

        while(remaining >= size){
            memcpy(((byte *)&slot), ptr, size);
            parity += slot;
            remaining -= size;
            ptr += size;
        }

        if(tail){
            memcpy(&slot, ptr, tail);
            pos = tail;
        }
    }

    if(pos != 0){
        parity += slot;
    }

    parity &= ~7;
    parity |= (v->total & 7);

    return parity;
}

boolean Parity_Compare(util par, void *sv){
    Abstract *a = (Abstract *)sv;
    if(a->type.of == TYPE_STRVEC){
        StrVec *v = (StrVec *)a;
        if((v->total & 7) != (par & 7)){
            return FALSE;
        }else{
            return Parity_FromVec(v) == par;
        }
    }else if(a->type.of == TYPE_STR){
        Str *s = (Str *)a;
        if((s->length & 7) != (par & 7)){
            return FALSE;
        }else{
            return Parity_From(s) == par;
        }
    }else{
        return FALSE;
    }
}

util Parity_FromBuff(Buff *bf){
    if(bf->st.st_size <= 0){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot create parity from Buff without a stat size", NULL);
        return 0;
    }
    util parity = 0;
    util slot = 0;
    util size = sizeof(util);

    i64 total = bf->st.st_size;

    Str *s = Str_Make(bf->m, STR_DEFAULT);
    while(total > 0 && (bf->type.state & END) == 0){
        if(total < (i64)s->length){
            s->length = (i16)total;
        }
        Buff_GetStr(bf, s);
        i64 remaining = STR_DEFAULT;
        byte *ptr = s->bytes;
        while(remaining >= size){
            memcpy(((byte *)&slot), ptr, size);
            parity += slot;
            remaining -= size;
            ptr += size;
        }
        total -= s->length;
    }

    parity &= ~7;
    parity |= (bf->st.st_size & 7);

    return parity;
}

util Parity_From(Str *s){
    util parity = 0;
    util slot = 0;
    word pos = 0;
    util size = sizeof(util);
    util mod = size-1;
    word remaining = s->length;
    byte *ptr = s->bytes;

    while(remaining >= size){
        memcpy(((byte *)&slot), ptr, size);
        parity += slot;
        remaining -= size;
        ptr += size;
    }

    if(remaining){
        slot = 0;
        memcpy(&slot, ptr, remaining);
        parity += slot;
    }

    parity &= ~7;
    parity |= (s->length & 7);

    return parity;
}

util Parity_Pre(Str *s){
    util parity = 0;
    util slot = 0;
    word pos = 0;
    util size = sizeof(util);
    util mod = size-1;
    word remaining = s->length;
    byte *ptr = s->bytes;

    while(remaining >= size){
        memcpy(((byte *)&slot), ptr, size);
        parity += slot;
        remaining -= size;
        ptr += size;
    }

    if(remaining){
        slot = 0;
        memcpy(&slot, ptr, remaining);
        parity += slot;
    }

    return parity;
}

util Parity_FromVec(StrVec *v){
    util parity = Parity_PreVec(v);
    return Parity_Finalize(parity, v->total);
}

util Parity_PreVec(StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    util parity = 0;
    util slot = 0;
    word pos = 0;
    util size = sizeof(util);
    util mod = size-1;

    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        word remaining = s->length;

        byte *ptr = s->bytes;
        if(pos > 0){
            word copy = min(remaining, size-pos);
            byte *sptr = (byte *)&slot;
            memcpy(sptr+pos, ptr, copy);
            remaining -= copy;
            if(remaining == 0 && pos < size){
                pos += copy;
                continue;
            }
            parity += slot;
            pos = 0;
            ptr += copy;
        }

        while(remaining >= size){
            memcpy(((byte *)&slot), ptr, size);
            parity += slot;
            remaining -= size;
            ptr += size;
        }

        if(remaining){
            slot = 0;
            memcpy(&slot, ptr, remaining);
            pos = remaining;
        }
    }

    if(pos != 0){
        parity += slot;
    }

    return parity;
}

util Parity_Finalize(util parity, i64 total){
    parity &= ~7;
    parity |= (total & 7);
    return parity;
}
