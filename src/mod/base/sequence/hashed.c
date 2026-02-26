/* Base.sequence.Hashed
 *
 * Key/Value Struct uses Parity for quick (but non-conclusive) hash comparison.
 *
 * Used in the Table object
 */

#include <external.h>
#include "base_module.h"

Lookup *HashLookup = NULL;

static util _Hash_Bytes(byte *bt, size_t length){
    Str s = {.type = {TYPE_STR, STRING_CONST},
        .length = length,
        .alloc = length,
        .bytes = bt
    };

    return Parity_From(&s);
}

static util Hash_Ptr(void *ptr){
	util h = 5381;
    return _Hash_Bytes((byte *)&ptr, sizeof(void *));
}

static util Hash_Str(void *a){
    Str *s = (Str *)a;
    return Parity_From(s);
}

static util Hash_StrVec(void *a){
    StrVec *v = (StrVec *)a;
    return Parity_FromVec(v);
}

static util Hash_Req(void *a){
    return 0;
}

static util Hash_Slab(void *a){
    return 0;
}

static util Hash_Span(void *a){
    return 0;
}

static util Hash_WI16(void *a){
    Single *sg = (Single *)a;
    return (util)sg->val.w;
}

static util Hash_Util(void *a){
    Single *sg = (Single *)a;
    return _Hash_Bytes((byte *)&sg->val.value, sizeof(util));
}

util Hash_Bytes(byte *bt, size_t length){
    return _Hash_Bytes(bt, length);
}

util Get_Hash(void *_a){
    Abstract *a = (Abstract *)_a;
    if(a->type.of == TYPE_HASHED){
        Hashed *h = (Hashed *)a;
        return h->id;
    }else if(a->type.of == TYPE_WRAPPED_UTIL || a->type.of == TYPE_WRAPPED_I64){
        return ((Single *)a)->val.value;
    }else if(a->type.of == TYPE_WRAPPED_U32 || a->type.of == TYPE_WRAPPED_I32){
        return (util)((Single *)a)->val.i;
    }else if(a->type.of == TYPE_STR){
        return Hash_Str(a);
    }else if(a->type.of == TYPE_STRVEC){
        return Hash_StrVec(a);
    }

    HashFunc func = (HashFunc)Lookup_Get(HashLookup, a->type.of);
    if(func != NULL){
        return func(a);
    }else{
        return Hash_Ptr((void *)a);
    }
}

boolean Hashed_Equals(Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    return Equals(a->key, b->key);
}

Hashed *Hashed_Make(MemCh *m, void *_a){
    Abstract *a = (Abstract *)_a;
    if(a != NULL && a->type.of == TYPE_HASHED){
        return (Hashed *)a;
    }
    Hashed *v = (Hashed *)MemCh_Alloc(m, sizeof(Hashed));
    v->type.of = TYPE_HASHED;
    if(a != NULL){
        v->id = Get_Hash(a);
        v->key = a;
    }
    return v;
}

Hashed *Hashed_Clone(MemCh *m, Hashed *oh){
    Hashed *h = MemCh_Realloc(m, sizeof(Hashed), oh, sizeof(Hashed));
    if(h->key != NULL && ((h->key = Clone(m, h->key)) == NULL)){
        return NULL;
    }
    if(h->value != NULL && ((h->value = Clone(m, h->key)) == NULL)){
        return NULL;
    }
    return h;
}

status Hash_Init(MemCh *m){
    if(HashLookup == NULL){
        status r = READY;
        HashLookup = Lookup_Make(m, _TYPE_ZERO);
        Lookup *lk = HashLookup;
        r |= Lookup_Add(m, lk, TYPE_STR, (void *)Hash_Str);
        r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)Hash_StrVec);
        r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
        r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
        r |= Lookup_Add(m, lk, TYPE_WRAPPED_I16, (void *)Hash_WI16);
        r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)Hash_Util);
        return r;
    }
    return NOOP;
}
