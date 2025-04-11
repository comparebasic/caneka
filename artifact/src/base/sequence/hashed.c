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

static util Hash_Req(Abstract *a){
    return 0;
}

static util Hash_Slab(Abstract *a){
    return 0;
}

static util Hash_Span(Abstract *a){
    return 0;
}

static status populateHash(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Hash_Abstract);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_PatMatch);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)Hash_PatCharDef);
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Hash_Str);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Hash_Req);
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
    return r;
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
    return Equals(a->item, (void *)b->item);
}

Hashed *Hashed_Make(MemCh *m, Abstract *a){
    if(a->type.of == TYPE_HASHED){
        return (Hashed *)a;
    }
    Hashed *v = (Hashed *)MemCh_Alloc(m, sizeof(Hashed));
    v->type.of = TYPE_HASHED;
    v->id = Get_Hash(a);
    v->item = a;
    return v;
}

Hashed *Hashed_Clone(MemCh *m, Hashed *oh){
    Hashed *h = MemCh_Realloc(m, sizeof(Hashed), oh, sizeof(Hashed));
    if(h->item != NULL && ((h->item = Clone(m, h->item)) == NULL)){
        return NULL;
    }
    if(h->value != NULL && ((h->value = Clone(m, h->item)) == NULL)){
        return NULL;
    }
    return h;
}


status Hash_Init(MemCh *m){
    if(HashLookup == NULL){
        HashLookup = Lookup_Make(m, _TYPE_START, populateHash, NULL);
        return SUCCESS;
    }
    return NOOP;
}
