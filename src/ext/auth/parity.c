#include <external.h>
#include <caneka.h>

boolean Parity_Compare(quad par, StrVec *v){
    if((v->total & 7) != (par & 7)){
        return FALSE;
    }else{
        return Parity_FromVec(v) == par;
    }
}

quad Parity_FromVec(StrVec *v){
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
