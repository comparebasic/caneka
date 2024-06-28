#include <external.h>
#include <caneka.h>

Chain *HashChain;

static util Hash_Bytes(byte *bt, size_t length){
	util h = 5381;
	util i = 0;
	byte c;
	while (i++ < length) {
		c = *bt++;
		h = (h << 5) + h + (h << 9) + h + (h << 31) + h + (h << 49) + h + c;
	}
    return h;
}

static util Hash_Abstract(Abstract *a){
    return 0;
}

static util Hash_Match(Abstract *a){
    return 0;
}

static util Hash_PatMatch(Abstract *a){
    return 0;
}

static util Hash_PatCharDef(Abstract *a){
    return 0;
}

static util Hash_String(Abstract *a){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    return Hash_Bytes(s->bytes, s->length);
}

static util Hash_StringFixed(Abstract *a){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    return Hash_Bytes(s->bytes, s->length);
}

static util Hash_SCursor(Abstract *a){
    return 0;
}

static util Hash_Range(Abstract *a){
    return 0;
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

static status populateHash(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Hash_Abstract);
    r |= Lookup_Add(m, lk, TYPE_MATCH, (void *)Hash_Match);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_PatMatch);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)Hash_PatCharDef);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_Match);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)Hash_String);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)Hash_StringFixed);
    r |= Lookup_Add(m, lk, TYPE_SCURSOR, (void *)Hash_SCursor);
    r |= Lookup_Add(m, lk, TYPE_RANGE, (void *)Hash_Range);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Hash_Req);
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
    return r;
}

util Get_Hash(Abstract *a){
    if(a->type.of == TYPE_HASHED){
        Hashed *h = (Hashed *)a;
        return h->id;
    }

    HashFunc func = (HashFunc)Chain_Get(HashChain, a->type.of);
    if(func != NULL){
        return func(a);
    }else{
        Fatal("Hash func for type not found", a->type.of);
        return 0;
    }
}

Hashed *Hashed_Make(MemCtx *m, Abstract *a){
    if(a->type.of == TYPE_HASHED){
        return (Hashed *)a;
    }
    Hashed *v = (Hashed *)MemCtx_Alloc(m, sizeof(Hashed));
    v->type.of = TYPE_HASHED;
    v->item = a;
    v->id = Get_Hash(a);
    return v;
}

status Hash_Init(MemCtx *m){
    if(HashChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateHash, NULL);
        HashChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}
