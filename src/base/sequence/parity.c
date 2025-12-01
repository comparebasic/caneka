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

boolean Parity_Compare(util par, StrVec *v){
    if((v->total & 7) != (par & 7)){
        return FALSE;
    }else{
        return Parity_FromVec(v) == par;
    }
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

util Parity_FromVec(StrVec *v){
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

    parity &= ~7;
    parity |= (v->total & 7);

    return parity;
}
