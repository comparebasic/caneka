#include <external.h>
#include <caneka.h>

Lookup *HashLookup = NULL;

static util _Hash_Bytes(byte *bt, size_t length, util h){
	util i = 0;
	byte c;
	while (i++ < length) {
		c = *bt;
		h = (h << 5) + h + (h << 9) + h + (h << 31) + h + (h << 49) + h + c;
        bt++;
	}
    return h;
}


static util Hash_Ptr(void *ptr){
	util h = 5381;
    return _Hash_Bytes((byte *)&ptr, sizeof(void *), h);
}

static util Hash_Abstract(Abstract *a){
    return 0;
}

static util Hash_PatMatch(Abstract *a){
    return 0;
}

static util Hash_PatCharDef(Abstract *a){
    return 0;
}

static util Hash_Str(Abstract *a){
    Str *s = (Str *)asIfc(a, TYPE_STR);
	util h = 5381;
    h = _Hash_Bytes(s->bytes, s->length, h);
    return h;
}

static util Hash_StrVec(Abstract *a){
    StrVec *v = (StrVec *)a;
	util h = 5381;
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        h = _Hash_Bytes(s->bytes, s->length, h);
    }

    return h;
}

static util Hash_Req(Abstract *a){
    return 0;
}

static util Hash_Slab(Abstract *a){
    return 0;
}

static util Hash_Span(Abstract *a){
    return 0;
}

static util Hash_WI16(Abstract *a){
    Single *sg = (Single *)a;
    return (util)sg->val.w;
}

util Hash_Bytes(byte *bt, size_t length){
	util h = 5381;
    return _Hash_Bytes(bt, length, h);
}

util Get_Hash(Abstract *a){
    if(a->type.of == TYPE_HASHED){
        Hashed *h = (Hashed *)a;
        return h->id;
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

Hashed *Hashed_Make(MemCh *m, Abstract *a){
    if(a->type.of == TYPE_HASHED){
        return (Hashed *)a;
    }
    Hashed *v = (Hashed *)MemCh_Alloc(m, sizeof(Hashed));
    v->type.of = TYPE_HASHED;
    v->id = Get_Hash(a);
    v->key = a;
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
        r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Hash_Abstract);
        r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_PatMatch);
        r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)Hash_PatCharDef);
        r |= Lookup_Add(m, lk, TYPE_STR, (void *)Hash_Str);
        r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)Hash_StrVec);
        r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Hash_Req);
        r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
        r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
        r |= Lookup_Add(m, lk, TYPE_WRAPPED_I16, (void *)Hash_WI16);
        return r;
    }
    return NOOP;
}
